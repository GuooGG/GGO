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
static uint64_t count = 0;

void func(){
    GGO_LOG_INFO(g_logger) << endl << "  name: " << GGo::Thread::GetThisName() << endl
                           << "  this.name: " << GGo::Thread::GetThis()->getName() <<endl
                           << "  id: " << GGo::GetThreadID() << endl 
                           << "  this.id: " << GGo::Thread::GetThis()->getID() << endl;
}

void countter(){
    for(int i = 0; i < 1000; i++){
        count++;
        GGO_LOG_INFO(g_logger) << count;
    }
}


int main(){
    GGO_LOG_INFO(g_logger) << "ThreadTest01 Start:";
    YAML::Node node = YAML::LoadFile("/root/workspace/GGoSeverFrame/conf/log.yml");
    GGo::Config::loadFromYaml(node);

    std::vector<GGo::Thread::ptr> threads;

    for(int i = 0 ;i < 100; i++){
        GGo::Thread::ptr thread(new GGo::Thread(&countter,"name_" + std::to_string(i)));
        threads.push_back(thread);
    }

    for (size_t i = 0; i < threads.size(); ++i)
    {
        threads[i]->join();
    }
    GGO_LOG_INFO(g_logger) << count;
    GGO_LOG_INFO(g_logger) << "ThreadTest01 end";
    return 0;
}