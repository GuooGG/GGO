#include"thread.h"
namespace GGo{

static thread_local Thread::ptr t_thread = nullptr;
static thread_local std::string t_thread_name = "UNKNOWN";

Thread::ptr Thread::GetThis(){
    return t_thread;

}

Thread::Thread(std::function<void()> cb, const std::string &name)
{
    if(name.empty()){
        m_name = "UNKNOWN";
    }

}

Thread::~Thread()
{
    if(m_thread){
        pthread_detach(m_thread);
    }
}

void Thread::join()
{
    if(m_thread){
        int rt = pthread_join(m_thread,nullptr);
        if(rt){
            //TODO::输出错误日志
        }
    }
    m_thread = 0;
}

void *Thread::run(void *arg)
{
    Thread::ptr thread = (Thread::ptr) arg;
    t_thread = thread;

}

const std::string &Thread::GetName()
{
    return t_thread_name;
}



}