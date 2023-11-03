#include"scheduler.h"
#include"fiber.h"
#include"thread.h"
#include"config.h"
#include"LogSystem.h"

GGo::Logger::ptr g_logger = GGO_LOG_NAME("system");
static uint64_t count = 0;


void printcount(){
    for(int i = 0; i < 3; i++){
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
    for(int i = 0;i < 5;i++){
        GGO_LOG_INFO(g_logger) << "mission " << i + 1 << " appended";
        scheduler->schedule(&printcount);
    }
    GGO_LOG_INFO(g_logger) << "scheduler stop";
    scheduler->stop();
    GGO_LOG_INFO(g_logger) << "scheduler stopped";
}

void test_fiber(){
    static int s_count = 10;
    GGO_LOG_INFO(g_logger) << "test in fiber s_count=" << s_count;

    // sleep(1);
    // GGo::Fiber::yieldToReady();
    if (--s_count >= 0)
    {
        GGo::Scheduler::getThis()->schedule(&test_fiber, GGo::GetThreadID());
    }
}
void test(){
    GGo::Scheduler sc(6, false, "test");
    sc.start();
    // sleep(2);
    sc.schedule(&test_fiber);
    sc.stop();
    return;

}
int main(){
    YAML::Node node = YAML::LoadFile("/root/workspace/GGoSeverFrame/Test/conf/log.yml");
    GGo::Config::loadFromYaml(node);

    test();
    return 0;
}