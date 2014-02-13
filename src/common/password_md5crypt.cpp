#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "password_md5crypt.h"
using namespace std;

//--------------------md5 begin--------------------
/*
 * The basic MD5 functions.
 *
 * F is optimized compared to its RFC 1321 definition just like in Colin
 * Plumb's implementation.
 */
#define F(x, y, z)			((z) ^ ((x) & ((y) ^ (z))))
#define G(x, y, z)			((y) ^ ((z) & ((x) ^ (y))))
#define H(x, y, z)			((x) ^ (y) ^ (z))
#define I(x, y, z)			((y) ^ ((x) | ~(z)))

/*
 * The MD5 transformation for all four rounds.
 */
#define STEP(f, a, b, c, d, x, t, s) \
	(a) += f((b), (c), (d)) + (x) + (t); \
	(a) = (((a) << (s)) | (((a) & 0xffffffff) >> (32 - (s)))); \
	(a) += (b);

/*
 * SET reads 4 input bytes in little-endian byte order and stores them
 * in a properly aligned word in host byte order.
 *
 * The check for little-endian architectures which tolerate unaligned
 * memory accesses is just an optimization.  Nothing will break if it
 * doesn't work.
 */
#if defined(__i386__) || defined(__x86_64__) || defined(__vax__)
#define SET(n) \
	(*(const unsigned long *)&ptr[(n) * 4])
#define GET(n) \
	SET(n)
#else
#define SET(n) \
	(ctx->block[(n)] = \
	(unsigned long)ptr[(n) * 4] | \
	((unsigned long)ptr[(n) * 4 + 1] << 8) | \
	((unsigned long)ptr[(n) * 4 + 2] << 16) | \
	((unsigned long)ptr[(n) * 4 + 3] << 24))
#define GET(n) \
	(ctx->block[(n)])
#endif

/*
 * This processes one or more 64-byte data blocks, but does NOT update
 * the bit counters.  There're no alignment requirements.
 */
static const void* body(struct md5_context *ctx, const void *data, size_t size)
{
	const unsigned char *ptr;
	unsigned long a, b, c, d;
	unsigned long saved_a, saved_b, saved_c, saved_d;

	ptr = (const unsigned char *)data;

	a = ctx->a;
	b = ctx->b;
	c = ctx->c;
	d = ctx->d;

	do {
		saved_a = a;
		saved_b = b;
		saved_c = c;
		saved_d = d;

/* Round 1 */
		STEP(F, a, b, c, d, SET(0), 0xd76aa478, 7)
		STEP(F, d, a, b, c, SET(1), 0xe8c7b756, 12)
		STEP(F, c, d, a, b, SET(2), 0x242070db, 17)
		STEP(F, b, c, d, a, SET(3), 0xc1bdceee, 22)
		STEP(F, a, b, c, d, SET(4), 0xf57c0faf, 7)
		STEP(F, d, a, b, c, SET(5), 0x4787c62a, 12)
		STEP(F, c, d, a, b, SET(6), 0xa8304613, 17)
		STEP(F, b, c, d, a, SET(7), 0xfd469501, 22)
		STEP(F, a, b, c, d, SET(8), 0x698098d8, 7)
		STEP(F, d, a, b, c, SET(9), 0x8b44f7af, 12)
		STEP(F, c, d, a, b, SET(10), 0xffff5bb1, 17)
		STEP(F, b, c, d, a, SET(11), 0x895cd7be, 22)
		STEP(F, a, b, c, d, SET(12), 0x6b901122, 7)
		STEP(F, d, a, b, c, SET(13), 0xfd987193, 12)
		STEP(F, c, d, a, b, SET(14), 0xa679438e, 17)
		STEP(F, b, c, d, a, SET(15), 0x49b40821, 22)

/* Round 2 */
		STEP(G, a, b, c, d, GET(1), 0xf61e2562, 5)
		STEP(G, d, a, b, c, GET(6), 0xc040b340, 9)
		STEP(G, c, d, a, b, GET(11), 0x265e5a51, 14)
		STEP(G, b, c, d, a, GET(0), 0xe9b6c7aa, 20)
		STEP(G, a, b, c, d, GET(5), 0xd62f105d, 5)
		STEP(G, d, a, b, c, GET(10), 0x02441453, 9)
		STEP(G, c, d, a, b, GET(15), 0xd8a1e681, 14)
		STEP(G, b, c, d, a, GET(4), 0xe7d3fbc8, 20)
		STEP(G, a, b, c, d, GET(9), 0x21e1cde6, 5)
		STEP(G, d, a, b, c, GET(14), 0xc33707d6, 9)
		STEP(G, c, d, a, b, GET(3), 0xf4d50d87, 14)
		STEP(G, b, c, d, a, GET(8), 0x455a14ed, 20)
		STEP(G, a, b, c, d, GET(13), 0xa9e3e905, 5)
		STEP(G, d, a, b, c, GET(2), 0xfcefa3f8, 9)
		STEP(G, c, d, a, b, GET(7), 0x676f02d9, 14)
		STEP(G, b, c, d, a, GET(12), 0x8d2a4c8a, 20)

/* Round 3 */
		STEP(H, a, b, c, d, GET(5), 0xfffa3942, 4)
		STEP(H, d, a, b, c, GET(8), 0x8771f681, 11)
		STEP(H, c, d, a, b, GET(11), 0x6d9d6122, 16)
		STEP(H, b, c, d, a, GET(14), 0xfde5380c, 23)
		STEP(H, a, b, c, d, GET(1), 0xa4beea44, 4)
		STEP(H, d, a, b, c, GET(4), 0x4bdecfa9, 11)
		STEP(H, c, d, a, b, GET(7), 0xf6bb4b60, 16)
		STEP(H, b, c, d, a, GET(10), 0xbebfbc70, 23)
		STEP(H, a, b, c, d, GET(13), 0x289b7ec6, 4)
		STEP(H, d, a, b, c, GET(0), 0xeaa127fa, 11)
		STEP(H, c, d, a, b, GET(3), 0xd4ef3085, 16)
		STEP(H, b, c, d, a, GET(6), 0x04881d05, 23)
		STEP(H, a, b, c, d, GET(9), 0xd9d4d039, 4)
		STEP(H, d, a, b, c, GET(12), 0xe6db99e5, 11)
		STEP(H, c, d, a, b, GET(15), 0x1fa27cf8, 16)
		STEP(H, b, c, d, a, GET(2), 0xc4ac5665, 23)

/* Round 4 */
		STEP(I, a, b, c, d, GET(0), 0xf4292244, 6)
		STEP(I, d, a, b, c, GET(7), 0x432aff97, 10)
		STEP(I, c, d, a, b, GET(14), 0xab9423a7, 15)
		STEP(I, b, c, d, a, GET(5), 0xfc93a039, 21)
		STEP(I, a, b, c, d, GET(12), 0x655b59c3, 6)
		STEP(I, d, a, b, c, GET(3), 0x8f0ccc92, 10)
		STEP(I, c, d, a, b, GET(10), 0xffeff47d, 15)
		STEP(I, b, c, d, a, GET(1), 0x85845dd1, 21)
		STEP(I, a, b, c, d, GET(8), 0x6fa87e4f, 6)
		STEP(I, d, a, b, c, GET(15), 0xfe2ce6e0, 10)
		STEP(I, c, d, a, b, GET(6), 0xa3014314, 15)
		STEP(I, b, c, d, a, GET(13), 0x4e0811a1, 21)
		STEP(I, a, b, c, d, GET(4), 0xf7537e82, 6)
		STEP(I, d, a, b, c, GET(11), 0xbd3af235, 10)
		STEP(I, c, d, a, b, GET(2), 0x2ad7d2bb, 15)
		STEP(I, b, c, d, a, GET(9), 0xeb86d391, 21)

		a += saved_a;
		b += saved_b;
		c += saved_c;
		d += saved_d;

		ptr += 64;
	} while (size -= 64);

	ctx->a = a;
	ctx->b = b;
	ctx->c = c;
	ctx->d = d;

	return ptr;
}

void safe_memset(void *data, int c, size_t size)
{
	volatile unsigned char *p = (volatile unsigned char *)data;

	for (; size > 0; size--)
		*p++ = (unsigned char)c;
}

void md5_init(struct md5_context *ctx)
{
	ctx->a = 0x67452301;
	ctx->b = 0xefcdab89;
	ctx->c = 0x98badcfe;
	ctx->d = 0x10325476;

	ctx->lo = 0;
	ctx->hi = 0;
}

void md5_update(struct md5_context *ctx, const void *data, size_t size)
{
	/* @UNSAFE */
	unsigned long saved_lo;
	unsigned long used, free;

	saved_lo = ctx->lo;
	if ((ctx->lo = (saved_lo + size) & 0x1fffffff) < saved_lo)
		ctx->hi++;
	ctx->hi += size >> 29;

	used = saved_lo & 0x3f;

	if (used) {
		free = 64 - used;

		if (size < free) {
			memcpy(&ctx->buffer[used], data, size);
			return;
		}

		memcpy(&ctx->buffer[used], data, free);
		data = (const unsigned char *) data + free;
		size -= free;
		body(ctx, ctx->buffer, 64);
	}

	if (size >= 64) {
		data = body(ctx, data, size & ~(unsigned long)0x3f);
		size &= 0x3f;
	}

	memcpy(ctx->buffer, data, size);
}

void md5_final(struct md5_context *ctx, unsigned char result[MD5_RESULTLEN])
{
	/* @UNSAFE */
	unsigned long used, free;

	used = ctx->lo & 0x3f;

	ctx->buffer[used++] = 0x80;

	free = 64 - used;

	if (free < 8) {
		memset(&ctx->buffer[used], 0, free);
		body(ctx, ctx->buffer, 64);
		used = 0;
		free = 64;
	}

	memset(&ctx->buffer[used], 0, free - 8);

	ctx->lo <<= 3;
	ctx->buffer[56] = ctx->lo;
	ctx->buffer[57] = ctx->lo >> 8;
	ctx->buffer[58] = ctx->lo >> 16;
	ctx->buffer[59] = ctx->lo >> 24;
	ctx->buffer[60] = ctx->hi;
	ctx->buffer[61] = ctx->hi >> 8;
	ctx->buffer[62] = ctx->hi >> 16;
	ctx->buffer[63] = ctx->hi >> 24;

	body(ctx, ctx->buffer, 64);

	result[0] = ctx->a;
	result[1] = ctx->a >> 8;
	result[2] = ctx->a >> 16;
	result[3] = ctx->a >> 24;
	result[4] = ctx->b;
	result[5] = ctx->b >> 8;
	result[6] = ctx->b >> 16;
	result[7] = ctx->b >> 24;
	result[8] = ctx->c;
	result[9] = ctx->c >> 8;
	result[10] = ctx->c >> 16;
	result[11] = ctx->c >> 24;
	result[12] = ctx->d;
	result[13] = ctx->d >> 8;
	result[14] = ctx->d >> 16;
	result[15] = ctx->d >> 24;

	safe_memset(ctx, 0, sizeof(*ctx));
}

void md5_get_digest(const void *data, size_t size,
					unsigned char result[MD5_RESULTLEN])
{
	struct md5_context ctx;

	md5_init(&ctx);
	md5_update(&ctx, data, size);
	md5_final(&ctx, result);
}
//--------------------md5 end  --------------------

static unsigned char itoa64[] =		/* 0 ... 63 => ascii - 64 */
	"./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static char	magic[] = "$1$";	/*
				 * This string is magic for
				 * this algorithm.  Having
				 * it this way, we can get
				 * get better later on
				 */

static void to64(string &str, unsigned long v, int n)
{
	while (--n >= 0) 
	{
		str += itoa64[v&0x3f];
		v >>= 6;
	}
}

/*
 * UNIX password
 *
 * Use MD5 for what it is best at...
 */

bool password_generate_md5_crypt(const char *pw, const char *salt, char *md5pass)
{
	if (pw == NULL || salt == NULL || md5pass == NULL)
	{
		return false;
	}

	const char *sp,*ep;
	unsigned char	final[MD5_RESULTLEN];
	int sl,pl,i,j;
	struct md5_context ctx,ctx1;
	unsigned long l;
	string passwd = "";
	size_t pw_len = strlen(pw);

	/* Refine the Salt first */
	sp = salt;

	/* If it starts with the magic string, then skip that */
	if (strncmp(sp, magic, sizeof(magic)-1) == 0)
		sp += sizeof(magic)-1;

	/* It stops at the first '$', max 8 chars */
	for(ep=sp;*ep && *ep != '$' && ep < (sp+8);ep++)
		continue;

	/* get the length of the true salt */
	sl = ep - sp;

	md5_init(&ctx);

	/* The password first, since that is what is most unknown */
	md5_update(&ctx,pw,pw_len);

	/* Then our magic string */
	md5_update(&ctx,magic,sizeof(magic)-1);

	/* Then the raw salt */
	md5_update(&ctx,sp,sl);

	/* Then just as many characters of the MD5(pw,salt,pw) */
	md5_init(&ctx1);
	md5_update(&ctx1,pw,pw_len);
	md5_update(&ctx1,sp,sl);
	md5_update(&ctx1,pw,pw_len);
	md5_final(&ctx1,final);
	for(pl = pw_len; pl > 0; pl -= MD5_RESULTLEN)
		md5_update(&ctx,final,pl>MD5_RESULTLEN ? MD5_RESULTLEN : pl);

	/* Don't leave anything around in vm they could use. */
	safe_memset(final, 0, sizeof(final));

	/* Then something really weird... */
	for (j=0,i = pw_len; i ; i >>= 1)
		if(i&1)
		    md5_update(&ctx, final+j, 1);
		else
		    md5_update(&ctx, pw+j, 1);

	/* Now make the output string */
	//passwd = t_str_new(sl + 64);
	//str_append(passwd, magic);
	passwd += magic;

	//str_append_n(passwd, sp, sl);
	passwd.append(sp, sl);

	//str_append_c(passwd, '$');
	passwd += '$';

	md5_final(&ctx,final);

	/*
	 * and now, just to make sure things don't run too fast
	 * On a 60 Mhz Pentium this takes 34 msec, so you would
	 * need 30 seconds to build a 1000 entry dictionary...
	 */
	for(i=0;i<1000;i++) {
		md5_init(&ctx1);
		if(i & 1)
			md5_update(&ctx1,pw,pw_len);
		else
			md5_update(&ctx1,final,MD5_RESULTLEN);

		if(i % 3)
			md5_update(&ctx1,sp,sl);

		if(i % 7)
			md5_update(&ctx1,pw,pw_len);

		if(i & 1)
			md5_update(&ctx1,final,MD5_RESULTLEN);
		else
			md5_update(&ctx1,pw,pw_len);
		md5_final(&ctx1,final);
	}

	l = (final[ 0]<<16) | (final[ 6]<<8) | final[12]; to64(passwd,l,4);
	l = (final[ 1]<<16) | (final[ 7]<<8) | final[13]; to64(passwd,l,4);
	l = (final[ 2]<<16) | (final[ 8]<<8) | final[14]; to64(passwd,l,4);
	l = (final[ 3]<<16) | (final[ 9]<<8) | final[15]; to64(passwd,l,4);
	l = (final[ 4]<<16) | (final[10]<<8) | final[ 5]; to64(passwd,l,4);
	l =                    final[11]                ; to64(passwd,l,2);

	/* Don't leave anything around in vm they could use. */
	safe_memset(final, 0, sizeof(final));

	strncpy(md5pass, passwd.c_str(), 63);
	return true;
}

bool generate_salt(unsigned len, char *ret_salt)
{
	if (len > 64 || ret_salt == NULL)
	{
		return false;
	}

	char saltlib[77] = 
		"23456789abcdefghjkmnpqrstuvwxyz23456789ABCDEFGHJKLMNPQRSTUVWXYZ23456789";
	int sliblen = strlen(saltlib);

	srand((unsigned)time(NULL));
	for (unsigned int i = 0; i < len; i++)
	{
		ret_salt[i] = saltlib[rand() % sliblen];
	}

	return true;
}

bool generate_md5_password(const char *pw, char *md5pass)
{
	if (pw != NULL)
	{
		char salt[9] = {0};
		if (generate_salt(8, salt))
		{
			return password_generate_md5_crypt(pw, salt, md5pass);
		}
	}
	return false;
}

bool check_password(const char *inputpw, const char *dbpw)
{
	char inputmd5pw[64] = {0};
	if (password_generate_md5_crypt(inputpw, dbpw, inputmd5pw))
	{
		if (strcmp(inputmd5pw, dbpw) == 0)
		{
			return true;
		}
	}
	
	return false;
}

bool Md5_salt(const std::string& password, std::string& md5_pass)
{
	char ch_salt[65] = "r4QD4vv9mb7wR9pcbLYfmA4YXm2NA8v9nafWf5R697ccg4wNky5r7sDK694N73dL";
	char ch_pass_md5[64] = "\0";
	if (!password_generate_md5_crypt(password.c_str(), ch_salt, ch_pass_md5))
	{
		return false;
	}

	md5_pass.assign(ch_pass_md5);

	return true;
}
