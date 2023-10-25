#include<thread.h>
#include<iostream>
#include"config.h"
#include<LogSystem.h>
#include<yaml-cpp/yaml.h>
#include<vector>
#include"util.h"
using std::cout;
using std::endl;

GGo::Logger::ptr g_logger = GGO_LOG_ROOT();

void func()
{
    GGO_LOG_INFO(g_logger) << endl
                           << "  name: " << GGo::Thread::GetThisName() << endl
                           << "  this.name: " << GGo::Thread::GetThis()->getName() << endl
                           << "  id: " << GGo::GetThreadID() << endl
                           << "  this.id: " << GGo::Thread::GetThis()->getID() << endl;
}
void test_thread_info(){
    std::vector<GGo::Thread::ptr> threads;

    for (int i = 0; i < 100; i++)
    {
        GGo::Thread::ptr thread(new GGo::Thread(&func, "name_" + std::to_string(i)));
        threads.push_back(thread);
    }

    for (size_t i = 0; i < threads.size(); ++i)
    {
        threads[i]->join();
    }
}

static uint64_t count = 0;
static GGo::Mutex mutex;
void countter()
{
    for (int i = 0; i < 50; i++)
    {
        count++;
        GGO_LOG_INFO(g_logger) << count;
    }
}
void test_countter_without_mutex(){
    std::vector<GGo::Thread::ptr> threads;

    for (int i = 0; i < 1000; i++)
    {
        GGo::Thread::ptr thread(new GGo::Thread(&countter, "name_" + std::to_string(i)));
        threads.push_back(thread);
    }

    for (size_t i = 0; i < threads.size(); ++i)
    {
        threads[i]->join();
    }
    GGO_LOG_INFO(g_logger) << count;
}
void countter_mutex()
{
    for (int i = 0; i < 100; i++)
    {
        GGo::Mutex::Lock lock(mutex);
        count++;
        GGO_LOG_INFO(g_logger) << count;
    }
}
void test_countter_with_mutex()
{
    std::vector<GGo::Thread::ptr> threads;

    for (int i = 0; i < 1000; i++)
    {
        GGo::Thread::ptr thread(new GGo::Thread(&countter_mutex, "name_" + std::to_string(i)));
        threads.push_back(thread);
    }

    for (size_t i = 0; i < threads.size(); ++i)
    {
        threads[i]->join();
    }
    GGO_LOG_INFO(g_logger) << count;
}



int main(){
    GGO_LOG_INFO(g_logger) << "ThreadTest01 Start:";
    YAML::Node node = YAML::LoadFile("/root/workspace/GGoSeverFrame/conf/log.yml");
    GGo::Config::loadFromYaml(node);

    test_countter_without_mutex();

    GGO_LOG_INFO(g_logger) << "ThreadTest01 end";
    return 0;
}