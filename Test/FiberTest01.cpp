#include<iostream>
#include"LogSystem.h"
#include"config.h"
#include"fiber.h"

GGo::Logger::ptr g_logger = GGO_LOG_ROOT();

void fiber_01(){
    GGO_LOG_INFO(g_logger) << "basic_01 begin";
    for(int i = 0; i < 10; i++){
        GGo::Fiber::yieldToReady();
    }
    GGO_LOG_INFO(g_logger) << "basic_01 end";
}
void test_fiber_basic_01(){
    GGo::Fiber::getThis();
    GGo::Fiber::ptr fiber(new GGo::Fiber(fiber_01));
    for(int i = 0; i <= 10; i++){
        fiber->swapIn();
        GGO_LOG_INFO(g_logger) << "basic_01 " <<"swapin " << i + 1 << " times ";
    }

}
int main(){
    GGO_LOG_INFO(g_logger) << "main begin";
    test_fiber_basic_01();
    GGO_LOG_INFO(g_logger) << "main end";
    return 0;
}