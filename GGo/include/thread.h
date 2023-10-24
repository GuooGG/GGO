//线程模块
#pragma once
#include<functional>
#include<memory>
/**
 * @file thread.h
 * @author GGo
 * @brief 线程模块
 * @date 2023-10-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include<thread>
#include<functional>
#include<pthread.h>

#include"nonCopyable.h"
#include"mutex.h"

namespace GGo{

//线程类型
class Thread : nonCopyable{
public:
	using ptr = std::shared_ptr<Thread>;

	/// @brief 构造函数
	/// @param cb 线程工作内容
	/// @param name 线程名称
	Thread(std::function<void()> cb,const std::string& name);

	/// @brief 析构函数
	~Thread();

	/// @brief 线程名称
	const std::string& getName() const {return m_name;}
	
	/// @brief 线程id
	pid_t getID() const {return m_id;}

	/// @brief 等待线程执行完成
	void join();

	/// @brief 获取当前线程指针 
	static Thread* GetThis();

	/// @brief 获取当前线程名称
	static const std::string& GetThisName();

	/// @brief 禁用移动构造函数
	Thread(const Thread &&) = delete;

	/// @brief 设置当前线程名称
	static void setName(const std::string& name);

private:

	/// @brief 线程执行
	static void* run(void* arg);
private:
	/// @brief 线程id
	pid_t m_id = -1;
	/// @brief 线程
	pthread_t m_thread = 0;
	/// @brief 线程工作内容
	std::function<void()> m_cb;
	/// @brief 线程名称
	std::string m_name;
	/// @brief 信号量
	Semaphore m_semaphore;

};

}
