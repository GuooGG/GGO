#include<iostream>
#include"LogSystem.h"
#include"config.h"
#include"fiber.h"

GGo::Logger::ptr g_logger = GGO_LOG_ROOT();

void fiber_01(){
    GGO_LOG_INFO(g_logger) << "basic_01 begin";
    GGo::Fiber::yieldToHold();
    GGO_LOG_INFO(g_logger) << "basic_01 end";
    GGo::Fiber::yieldToHold();
}
void test_fiber_basic_01(){
    GGo::Fiber::ptr main_fiber =  GGo::Fiber::getThis();
    GGo::Fiber::ptr fiber(new GGo::Fiber(fiber_01));
    fiber->swapIn();
    GGO_LOG_INFO(g_logger) << "after swapIn once";
    fiber->swapIn();
    GGO_LOG_INFO(g_logger) << "after swapIn twice";
}
int main(){
    GGO_LOG_INFO(g_logger) << "main begin";
    test_fiber_basic_01();
    GGO_LOG_INFO(g_logger) << "main end";
    return 0;
}