#pragma once
#include <thread>
#include <atomic>
#include <string>
#include "delegate.h"
template <class T, class R, class ... P>
class threadPool
{
private:
	enum THREAD_STATUS {
		T_IDLE,
		T_RUNNING,
		T_JOIN_ABLE,
		T_DO_JOIN
	};
	std::thread* m_threads;
	std::atomic<char>* m_threadStatus;
	uint16_t m_maxThreads;
	uint16_t m_currentMaxThreads;
	std::atomic < uint16_t> m_currentThreadCount;
	_mem_delegateZ<T, R, P...> m_delegate;
	std::string m_name;
private:

	static R wrap(_mem_delegateZ<T, R, P...>* d, P&& ... args)
	{
		return (*d)(std::forward<P>(args) ...);
	}
	bool run(int id, P&& ...args)
	{
		char status = T_IDLE;
		if (m_threadStatus[id].compare_exchange_strong(status, T_RUNNING))
		{
			if (++m_currentThreadCount > m_currentMaxThreads)
			{
				m_currentThreadCount--;
				m_threadStatus[id] = T_IDLE;
				return false;
			}
			m_threads[id] = std::thread(wrap, &m_delegate, std::forward<P>(args) ...);
			return true;
		}
		else
			return false;
	}
public:
	threadPool(uint16_t maxThread, uint16_t currentMaxThread, T* t, R(T::* f)(P...), const char* name) :m_maxThreads(maxThread), m_delegate(create_delegate(t, f)), m_name(name)
	{
		m_threads = new std::thread[maxThread];
		m_threadStatus = new std::atomic<char>[maxThread];
		for (int i = 0; i < maxThread; i++)
			m_threadStatus[i].store(0, std::memory_order_relaxed);
		if (currentMaxThread > maxThread)
			m_currentMaxThreads = maxThread;
		else
			m_currentMaxThreads = currentMaxThread;
		m_currentThreadCount = 0;
	}
	threadPool(const threadPool& pool) :m_delegate(pool.m_delegate)
	{
		m_name = pool.m_name;
		m_maxThreads = pool.m_maxThreads;
		m_currentMaxThreads = pool.m_currentMaxThreads;
		m_threads = new std::thread[m_maxThreads];
		m_threadStatus = new std::atomic<char>[m_maxThreads];
		for (int i = 0; i < m_maxThreads; i++)
			m_threadStatus[i].store(0, std::memory_order_relaxed);
		m_currentThreadCount = 0;
	}
	threadPool& operator=(const threadPool& pool)
	{
		m_name = pool.m_name;
		m_maxThreads = pool.m_maxThreads;
		m_currentMaxThreads = pool.m_currentMaxThreads;
		m_threads = new std::thread[m_maxThreads];
		m_threadStatus = new std::atomic<char>[m_maxThreads];
		for (int i = 0; i < m_maxThreads; i++)
			m_threadStatus[i].store(0, std::memory_order_relaxed);
		m_delegate = pool.m_delegate;
		m_currentThreadCount = 0;
		return *this;
	}
	~threadPool()
	{
		for (uint16_t i = 0; i < m_maxThreads; i++)
		{
			if (m_threadStatus[i].load(std::memory_order_relaxed) != T_IDLE)
			{
				m_threads[i].join();
			}
		}
		delete[]m_threads;
		delete[]m_threadStatus;
	}
	void updateCurrentMaxThread(uint16_t currentMaxThread)
	{
		if (currentMaxThread > m_maxThreads)
			m_currentThreadCount = m_maxThreads;
		else
			m_currentThreadCount = currentMaxThread;
	}
	bool createNewThread(P&& ...argv)
	{
		if (m_currentThreadCount.load(std::memory_order_relaxed) >= m_currentMaxThreads)
			return false;
		for (uint16_t i = 0; i < m_maxThreads; i++)
		{
			char status = m_threadStatus[i].load(std::memory_order_relaxed);
			if (status == T_IDLE)
			{
				if (run(i, std::forward<P>(argv) ...))
					return true;
			}
			else if (status == T_JOIN_ABLE)
			{
				if (m_threadStatus[i].compare_exchange_strong(status, T_DO_JOIN))
				{
					m_threads[i].join();
					m_threadStatus[i].store(T_IDLE, std::memory_order_relaxed);
					if (run(i, std::forward<P>(argv) ...))
						return true;
				}
			}
		}
		return false;
	}
};
template <class T, class R, typename... P>
static threadPool<T, R, P...> createThreadPool(uint16_t maxThread, T* t, R(T::* f)(P...), const char* name)
{
	return threadPool<T, R, P...>(maxThread, 1, t, f, name);
}
