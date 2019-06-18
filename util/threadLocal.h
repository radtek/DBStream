#pragma once
#include <string.h>
#include"likely.h"
#include "winDll.h"
class threadLocalWrap {
public:
	threadLocalWrap(); //we use this function to init threadid
	~threadLocalWrap();
	inline void idle() {}//do no thing ,thread_local varaiable will init when use it first
};
#ifdef OS_WIN
DLL_EXPORT int getThreadId();
DLL_EXPORT threadLocalWrap& getThreadLocalWrap();
#else
extern thread_local int threadid;
extern thread_local threadLocalWrap _threadLocalWrap;
#define  getThreadId() threadid
#define getThreadLocalWrap() _threadLocalWrap
#endif
static constexpr int maxThreadCount = 256;

DLL_EXPORT void registerThreadLocalVar(void (*_unset)(void* v), void* v);
DLL_EXPORT void destroyThreadLocalVar(void* v);

template<class T>
class DLL_EXPORT threadLocal
{
protected:
	T* m_var[maxThreadCount];
public:
	threadLocal()
	{
		memset(m_var, 0, sizeof(m_var));
		registerThreadLocalVar(_unset, this);
	}
	virtual ~threadLocal() 
	{
		destroyThreadLocalVar(this);
		for (int i = 0; i < maxThreadCount; i++)
		{
			if (m_var[i] != nullptr)
				delete m_var[i];
		}
	}
	inline T* get()
	{
		getThreadLocalWrap().idle();
		return m_var[getThreadId()];
	}
	inline void set(T* v)
	{
		getThreadLocalWrap().idle();
		m_var[getThreadId()] = v;
	}
	inline void unset()
	{
		getThreadLocalWrap().idle();
		if (likely(m_var[getThreadId()] != nullptr))
		{
			delete m_var[getThreadId()];
			m_var[getThreadId()] = nullptr;
		}
	}
	static void _unset(void * v)
	{
		static_cast<threadLocal<T>*>(v)->unset();
	}
};

