#pragma once
#ifdef OS_WIN
#include <shared_mutex>
typedef std::shared_mutex shared_mutex;
#else
#ifdef OS_LINUX
#include <pthread.h>
struct shared_mutex
{
	pthread_rwlock_t mlock;
	shared_mutex()
	{
		pthread_rwlock_init(&mlock,nullptr);
	}
	~shared_mutex()
	{
		pthread_rwlock_destroy(&mlock);
	}
	void unlock()
	{
		pthread_rwlock_unlock(&mlock);
	}
	void unlock_shared()
	{
		pthread_rwlock_unlock(&mlock);
	}
	inline void lock()
	{
		pthread_rwlock_wrlock(&mlock);
	}
	inline bool try_lock()
	{
		return 0==pthread_rwlock_trywrlock(&mlock);
	}
	inline void lock_shared()
	{
		pthread_rwlock_rdlock(&mlock);
	}
	inline bool try_lock_shared()
	{
		return 0==pthread_rwlock_tryrdlock(&mlock);
	}
};
#endif
#endif
