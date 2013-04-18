#include <boost/thread.hpp>
#include <map>

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
