//线程库
#pragma once
#include<thread>
#include<functional>
#include<memory>

namespace GGo {

class Thread {
public:
	using ptr = std::shared_ptr<Thread>;


private:
	//禁止线程拷贝
	Thread(const Thread&) = delete;
	Thread(const Thread&&) = delete;
	Thread& operator=(const Thread&) = delete;

private:

};

}