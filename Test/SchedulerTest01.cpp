#include"scheduler.h"
#include"fiber.h"
#include"thread.h"
#include"config.h"
#include"LogSystem.h"

GGo::Logger::ptr g_logger = GGO_LOG_NAME("system");
static uint64_t count = 0;


void printcount(){
    count++;
    GGO_LOG_INFO(g_logger) << "count now= " << count;
}
void test_scheduler(){
    GGo::Scheduler::ptr scheduler(new GGo::Scheduler(10, false, "test scheduler"));
    GGO_LOG_INFO(g_logger) << "scheduler structed";
    GGO_LOG_INFO(g_logger) << "scheduler start";
    scheduler->start();
    GGO_LOG_INFO(g_logger) << "scheduler started";
    for(int i = 0;i<1000;i++){
        scheduler->schedule(&printcount);
    }
    GGO_LOG_INFO(g_logger) << "scheduler stopp";
    scheduler->stop();
    GGO_LOG_INFO(g_logger) << "scheduler stopped";
}

int main(){
    YAML::Node node = YAML::LoadFile("/root/workspace/GGoSeverFrame/Test/conf/log.yml");
    GGo::Config::loadFromYaml(node);

    test_scheduler();
    return 0;
}