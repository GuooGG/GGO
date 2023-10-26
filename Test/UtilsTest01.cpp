/**
 * @file utilsTest.cpp
 * @author GGo
 * @brief 测试utils.h模块
 * @date 2023-10-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include<iostream>
#include<yaml-cpp/yaml.h>
#include "util.h"
#include "LogSystem.h"
#include"config.h"
using std::cout;
using std::endl;

GGo::Logger::ptr g_logger = GGO_LOG_NAME("system");

/// @brief 测试返回调用栈信息
void func4(){
    GGO_LOG_INFO(g_logger) << endl << GGo::backTraceToString(100, 0, "");
}
void func3(){
    func4();
}
void func2(){
    func3();
}
void func1(){
    func2();
}
void func0(){
    func1();
}
void test_backtrace(){
    func0();
}

int main(){
    YAML::Node node = YAML::LoadFile("/root/workspace/GGoSeverFrame/Test/conf/log.yml");
    GGo::Config::loadFromYaml(node);
    test_backtrace();
    return 0;
}