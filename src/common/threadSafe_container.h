#include <boost/thread.hpp>
#include <list>
#include <vector>
#include <map>

template<typename T>
class CThreadSafeList
{

public:
	CThreadSafeList() {}
	~CThreadSafeList() 
	{
		if (!list_.empty())
		{
			list_.clear();
		}

	}

	void push_back(const T &pt) 
	{
		boost::mutex::scoped_lock oLock(mutex_);
		list_.push_back(pt);
	}

	T pop_front() 
	{
		boost::mutex::scoped_lock oLock(mutex_);
		if (list_.size() > 0) 
		{
			T oThread = list_.front();
			list_.pop_front();
			return oThread;
		}

		return t_object_;
	}

	void earse(T &Object) 
	{
		boost::mutex::scoped_lock oLock(mutex_);
		typedef typename std::list<T>::iterator iter_thread;
		for (iter_thread it = list_.begin(); it != list_.end();) 
		{
			if (Object == *it) 
			{
				list_.erase(it++);
				break;
			}
			else 
			{
				++it;
			}
		}
	}

	void clear()
	{    
		boost::mutex::scoped_lock oLock(mutex_);
		if (!list_.empty())
		{
			list_.clear();
		}

		return;
	}

	int size() 
	{
		boost::mutex::scoped_lock oLock(mutex_);
		return list_.size();
	}

	bool empty()
	{
		boost::mutex::scoped_lock oLock(mutex_);
		return list_.empty();
	}

public:
	boost::mutex mutex_;

private:
	std::list<T> list_;
	T t_object_;
};

template<typename T>
class CThreadSafeVector
{

public:
	CThreadSafeVector() {}
	~CThreadSafeVector()
	{
		if (!vector_.empty())
		{
			vector_.clear();
		}

	}

	void push_back(const T &pt)
	{
		boost::mutex::scoped_lock oLock(mutex_);
		vector_.push_back(pt);
	}

	T at(const unsigned int i)
	{
		boost::mutex::scoped_lock oLock(mutex_);
		if (vector_.size() > 0)
		{
			return vector_.at(i);
		}

		return t_object_;
	}

	T pop_front()
	{
		boost::mutex::scoped_lock oLock(mutex_);
		if (vector_.size() > 0)
		{
			T oThread = vector_.front();
			vector_.pop_front();
			return oThread;
		}

		return t_object_;
	}

	void earse(T &Object)
	{
		boost::mutex::scoped_lock oLock(mutex_);
		typedef typename std::vector<T>::iterator iter_thread;
		for (iter_thread it = vector_.begin(); it != vector_.end();)
		{
			if (Object == *it)
			{
				vector_.erase(it++);
				break;
			}
			else
			{
				++it;
			}
		}
	}

	void clear()
	{
		boost::mutex::scoped_lock oLock(mutex_);
		if (!vector_.empty())
		{
			vector_.clear();
		}

		return;
	}

	int size()
	{
		boost::mutex::scoped_lock oLock(mutex_);
		return vector_.size();
	}

	bool empty()
	{
		boost::mutex::scoped_lock oLock(mutex_);
		return vector_.empty();
	}

public:
	boost::mutex mutex_;

private:
	std::vector<T> vector_;
	T t_object_;
};

template<typename K, typename V>
class CThreadSafeMap
{

public:
	CThreadSafeMap() {}
	~CThreadSafeMap() 
	{
		if (!map_.empty())
		{
			map_.clear();
		}
	}

	void insert(const K& key, const V& value)
	{
		boost::mutex::scoped_lock oLock(mutex_);
		map_.insert(std::pair<K, V>(key, value));
	}

	V find(const K& key)
	{
		boost::mutex::scoped_lock oLock(mutex_);
		if (map_.size() > 0)
		{
			typedef typename std::map<K, V>::iterator iter_thread;
			iter_thread iter= map_.find(key);
			if(iter != map_.end())
				return iter->second;
		}

		return value_;
	}

	void earse(const K& key)
	{
		boost::mutex::scoped_lock oLock(mutex_);
		typedef typename std::map<K, V>::iterator iter_thread;
		iter_thread iter= map_.find(key);
		if(iter != map_.end())
			map_.erase(iter);
	}

	void clear()
	{
		boost::mutex::scoped_lock oLock(mutex_);
		if (!map_.empty())
		{
			map_.clear();
		}

		return;
	}

	int size()
	{
		boost::mutex::scoped_lock oLock(mutex_);
		return map_.size();
	}

	bool empty()
	{
		boost::mutex::scoped_lock oLock(mutex_);
		return map_.empty();
	}

public:
	boost::mutex mutex_;

private:
	std::map<K, V> map_;
	V value_;
};
