#include<thread.h>
#include<iostream>
#include<logSystem.h>
#include<yaml-cpp/yaml.h>
#include<vector>
#include "GGo.h"
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





void config_test()
{
    GGo::Config::visit([](GGo::ConfigVarBase::ptr ba)
                       { GGO_LOG_INFO(g_logger) << "name= " << ba->getName() << "description= " << ba->getDescription(); });
}
void test_config_thread_security(){
    GGo::ConfigVar<int>::ptr g_int_value_config =
        GGo::Config::Lookup("system.port", (int)8080, "system.port");
    GGo::ConfigVar<float>::ptr g_float_value_config =
        GGo::Config::Lookup("system.value", (float)100.2f, "system.value");
    GGo::ConfigVar<std::vector<int>>::ptr g_int_vec_value_config =
        GGo::Config::Lookup("system.intvec", std::vector<int>{7, 9}, "system.intvec");
    GGo::ConfigVar<std::list<std::string>>::ptr g_str_list_value_config =
        GGo::Config::Lookup("system.strlist", std::list<std::string>{"h", "w"}, "system.strlist");
    GGo::ConfigVar<std::set<int>>::ptr g_int_set_value_config =
        GGo::Config::Lookup("system.intset", std::set<int>{6, 6, 8}, "system.intset");
    GGo::ConfigVar<std::unordered_set<int>>::ptr g_int_uset_value_config =
        GGo::Config::Lookup("system.intuset", std::unordered_set<int>{4, 6}, "system.intuset");
    GGo::ConfigVar<std::map<std::string, int>>::ptr g_str_int_map_value_config =
        GGo::Config::Lookup("system.strintmap", std::map<std::string, int>{{"k", 2}}, "system.strintmap");
    GGo::ConfigVar<std::unordered_map<std::string, int>>::ptr g_str_int_umap_value_config =
        GGo::Config::Lookup("system.strintumap", std::unordered_map<std::string, int>{{"g", 3}}, "system.strintumap");
    YAML::Node node = YAML::LoadFile("/root/workspace/GGoSeverFrame/Test/conf/log.yml");
    GGo::Config::loadFromYaml(node);

    std::vector<GGo::Thread::ptr> threads;

    for (int i = 0; i < 1000; i++)
    {
        GGo::Thread::ptr thread(new GGo::Thread(&config_test, "name_" + std::to_string(i)));
        threads.push_back(thread);
    }

    for (size_t i = 0; i < threads.size(); ++i)
    {
        threads[i]->join();
    }
}

int main(){
    GGO_LOG_INFO(g_logger) << "ThreadTest01 Start:";
    YAML::Node node = YAML::LoadFile("/root/workspace/GGoSeverFrame/conf/log.yml");
    GGo::Config::loadFromYaml(node);

    test_config_thread_security();

    GGO_LOG_INFO(g_logger) << "ThreadTest01 end";
    return 0;
}