//线程模块
#pragma once
#include<thread>
#include<functional>
#pragma once
#include<memory>
#include<thread>
#include<functional>
#include<pthread.h>

namespace GGo {

//线程类型
class Thread {
public:
	/// @brief 智能指针
	using ptr = std::shared_ptr<Thread>;
	Thread(std::function<void()> cb,const std::string& name);
	~Thread();

	const std::string& getName() const {return m_name;}
	pid_t getID() const {return m_id;}

	void join();
	void* run(void* arg);

	static Thread::ptr GetThis();
	static const std::string& GetName();

private:
	/// 保证线程无法被默认拷贝 
	Thread(const Thread&) = delete; 
	Thread(const Thread&&) = delete;
	Thread& operator=(const Thread&) = delete;

private:
	pid_t m_id;
	pthread_t m_thread;
	std::function<void()> m_cb;
	std::string m_name;

};

}
