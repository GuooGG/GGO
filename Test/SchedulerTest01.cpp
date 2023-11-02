#include"scheduler.h"
#include"fiber.h"
#include"thread.h"
#include"config.h"
#include"LogSystem.h"

GGo::Logger::ptr g_logger = GGO_LOG_NAME("system");
static uint64_t count = 0;


void printcount(){
    for(int i = 0; i < 10; i++){
        count++;
        GGO_LOG_INFO(g_logger) << "count now= " << count;
        GGo::Fiber::yieldToReady();

    }
}
void test_scheduler(bool use_caller){
    GGo::Scheduler::ptr scheduler(new GGo::Scheduler(2, use_caller, "test scheduler"));
    GGO_LOG_INFO(g_logger) << "scheduler structed";
    GGO_LOG_INFO(g_logger) << "scheduler start";
    scheduler->start();
    GGO_LOG_INFO(g_logger) << "scheduler started";
    for(int i = 0;i < 10;i++){
        GGO_LOG_INFO(g_logger) << "mission " << i + 1 << " appended";
        scheduler->schedule(&printcount);
    }
    GGO_LOG_INFO(g_logger) << "scheduler stop";
    scheduler->stop();
    GGO_LOG_INFO(g_logger) << "scheduler stopped";
}

int main(){
    YAML::Node node = YAML::LoadFile("/root/workspace/GGoSeverFrame/Test/conf/log.yml");
    GGo::Config::loadFromYaml(node);

    test_scheduler(true);
    return 0;
}