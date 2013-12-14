/*
 ============================================================================
 Name        : testTimer.c
 Author      : http://www.ibm.com/developerworks/cn/linux/l-cn-timers/
 ============================================================================
 */

/*
 * This file provides an interface of timers with signal and list
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/queue.h>

#define MAX_TIMER_NUM		1000	/**< max timer number	*/
#define TIMER_START 		1	/**< timer start(second)*/
#define TIMER_TICK 		1	/**< timer tick(second)	*/
#define INVALID_TIMER_ID 	(-1)	/**< invalid timer ID	*/

#ifndef TRUE
#define TRUE 	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

typedef int timer_id;

/**
 * The type of callback function to be called by timer scheduler when a timer
 * has expired.
 *
 * @param id		The timer id.
 * @param user_data     The user data.
 * $param len		The length of user data.
 */
typedef int timer_expiry(timer_id id, void *user_data, int len);

/**
 * The type of the timer
 */
struct timer {
	LIST_ENTRY(timer) entries;	/**< list entry		*/

	timer_id id;			/**< timer id		*/

	int interval;			/**< timer interval(second)*/
	int elapse; 			/**< 0 -> interval 	*/

	timer_expiry *cb;		/**< call if expiry 	*/
	void *user_data;		/**< callback arg	*/
	int len;			/**< user_data length	*/
};

/**
 * The timer list
 */
struct timer_list {
	LIST_HEAD(listheader, timer) header;	/**< list header 	*/
	int num;				/**< timer entry number */
	int max_num;				/**< max entry number	*/

	void (*old_sigfunc)(int);		/**< save previous signal handler */
	void (*new_sigfunc)(int);		/**< our signal handler	*/

	struct itimerval ovalue;		/**< old timer value */
	struct itimerval value;			/**< our internal timer value */
};

int init_timer(int count);

int destroy_timer(void);

timer_id
add_timer(int interval, timer_expiry *cb, void *user_data, int len);

int del_timer(timer_id id);

static struct timer_list timer_list;

static void sig_func(int signo);

/**
 * Create a timer list.
 *
 * @param count    The maximum number of timer entries to be supported
 *			initially.
 *
 * @return         0 means ok, the other means fail.
 */
int init_timer(int count)
{
	int ret = 0;

	if(count <=0 || count > MAX_TIMER_NUM) {
		printf("the timer max number MUST less than %d.\n", MAX_TIMER_NUM);
		return -1;
	}

	memset(&timer_list, 0, sizeof(struct timer_list));
	LIST_INIT(&timer_list.header);
	timer_list.max_num = count;

	/* Register our internal signal handler and store old signal handler */
	if ((timer_list.old_sigfunc = signal(SIGALRM, sig_func)) == SIG_ERR) {
		return -1;
	}
	timer_list.new_sigfunc = sig_func;

	/* Setting our interval timer for driver our mutil-timer and store old timer value */
	timer_list.value.it_value.tv_sec = TIMER_START;
	timer_list.value.it_value.tv_usec = 0;
	timer_list.value.it_interval.tv_sec = TIMER_TICK;
	timer_list.value.it_interval.tv_usec = 0;
	ret = setitimer(ITIMER_REAL, &timer_list.value, &timer_list.ovalue);

	return ret;
}

/**
 * Destroy the timer list.
 *
 * @return          0 means ok, the other means fail.
 */
int destroy_timer(void)
{
	struct timer *node = NULL;

	if ((signal(SIGALRM, timer_list.old_sigfunc)) == SIG_ERR) {
		return -1;
	}

	if((setitimer(ITIMER_REAL, &timer_list.ovalue, &timer_list.value)) < 0) {
		return -1;
	}

	while (!LIST_EMPTY(&timer_list.header)) {/* Delete. */
		node = LIST_FIRST(&timer_list.header);
	LIST_REMOVE(node, entries);
		/* Free node */
		printf("Remove id %d\n", node->id);
		free(node->user_data);
		free(node);
	}

	memset(&timer_list, 0, sizeof(struct timer_list));

	return 0;
}

/**
 * Add a timer to timer list.
 *
 * @param interval  The timer interval(second).
 * @param cb  	    When cb!= NULL and timer expiry, call it.
 * @param user_data Callback's param.
 * @param len  	    The length of the user_data.
 *
 * @return          The timer ID, if == INVALID_TIMER_ID, add timer fail.
 */
timer_id
add_timer(int interval, timer_expiry *cb, void *user_data, int len)
{
	struct timer *node = NULL;

	if (cb == NULL || interval <= 0) {
		return INVALID_TIMER_ID;
	}

	if(timer_list.num < timer_list.max_num) {
		timer_list.num++;
	} else {
		return INVALID_TIMER_ID;
	}

	if((node = malloc(sizeof(struct timer))) == NULL) {
		return INVALID_TIMER_ID;
	}
	if(user_data != NULL || len != 0) {
		node->user_data = malloc(len);
		memcpy(node->user_data, user_data, len);
		node->len = len;
	}

	node->cb = cb;
	node->interval = interval;
	node->elapse = 0;
	node->id = timer_list.num;

	LIST_INSERT_HEAD(&timer_list.header, node, entries);

	return node->id;
}

/**
 * Delete a timer from timer list.
 *
 * @param id  	    The timer ID.
 *
 * @return          0 means ok, the other fail.
 */
int del_timer(timer_id id)
{
	if (id <0 || id > timer_list.max_num) {
		return -1;
	}

	struct timer *node = timer_list.header.lh_first;
	for ( ; node != NULL; node = node->entries.le_next) {
		printf("Total timer num %d/timer id %d.\n", timer_list.num, id);
		if (id == node->id) {
			LIST_REMOVE(node, entries);
			timer_list.num--;

			free(node->user_data);
			free(node);
			return 0;
		}
	}

	/* Can't find the timer */
	return -1;
}

/* Tick Bookkeeping */
static void sig_func(int signo)
{
	struct timer *node = timer_list.header.lh_first;
	for ( ; node != NULL; node = node->entries.le_next) {
		node->elapse++;
		if(node->elapse >= node->interval) {
			node->elapse = 0;
			node->cb(node->id, node->user_data, node->len);
		}
	}
}

static char *fmt_time(char *tstr)
{
	time_t t;

	t = time(NULL);
	strcpy(tstr, ctime(&t));
	tstr[strlen(tstr)-1] = '\0';

	return tstr;
}

/* Unit Test */
timer_id id[2], call_cnt = 0;
int timer1_cb(timer_id id, void *arg, int len)
{
	char tstr[200];
	static int i, ret;

	/* XXX: Don't use standard IO in the signal handler context, I just use it demo the timer */
	printf("hello [%s]/id %d: timer1_cb is here.\n", fmt_time(tstr),id);
	if (i > 10) {
		ret = del_timer(id);
		printf("timer1_cb: %s del_timer/id %d::ret=%d\n", fmt_time(tstr),id,ret);
	}
	i++;
	call_cnt++;

	return 0;
}

int timer2_cb(timer_id id, void *arg, int len)
{
	char tstr[200];
	static int i, ret;

	printf("hello [%s]/id %d: timer2_cb is here.\n", fmt_time(tstr),id);
	if (i > 10) {
		ret = del_timer(id);
		printf("timer2_cb: %s del_timer/id %d::ret=%d\n", fmt_time(tstr),id,ret);
	}
	i++;
	call_cnt++;

	return 0;
}

int main(void)
{
	char arg[50];
	char tstr[200];
	int ret;
	struct timespec time;
	time.tv_sec = 0,
	time.tv_nsec= 100000000;

	init_timer(MAX_TIMER_NUM);

	id[0] = add_timer(1, timer1_cb, NULL, 0);
	printf("Register timer id[0]=%d\n", id[0]);

	id[1] = add_timer(2, timer2_cb, arg, 50);
	printf("Register timer id[1]=%d\n", id[1]);

	while (1) {
		if (call_cnt > 10) {
			break;
		}

		nanosleep(&time, NULL);
	}

	ret = destroy_timer();
	printf("main: %s destroy_timer, ret=%d\n", fmt_time(tstr), ret);
	return 0;
}
