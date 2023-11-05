#include<iostream>
#include"logSystem.h"

int main(int argc,char** argv){
    //创建日志器
    GGo::Logger::ptr logger(new GGo::Logger);
    //添加控制台日志目标
    logger->addAppender(GGo::LogAppender::ptr(new GGo::StdoutLogAppender));
    //添加文件输出日志目标
    GGo::FileLogAppender::ptr file_appender(new GGo::FileLogAppender("/root/workspace/GGoSeverFrame/log/Testlog.txt"));
    //设置新的格式器格式
    GGo::LogFormatter::ptr fmt(new GGo::LogFormatter("%d%T%p%T%m%n"));
    //给文件输出日志设定新的输出格式
    file_appender->setFormatter(fmt);
    //给文件输出日志设定日志级别
    file_appender->setLevel(GGo::LogLevel::FATAL);
    //为日志器添加文件输出
    logger->addAppender(file_appender);
    //开始测试
    std::cout<<"hello GGo log"<<std::endl;

    std::cout<<"logger_level : " << "DEBUG " <<std::endl;
    std::cout<<"file_level: " << " ERROR " <<std::endl;
    GGO_LOG_DEBUG(logger) << "logger debug01";
    GGO_LOG_INFO(logger) << "logger info01";
    GGO_LOG_WARN(logger) << "logger warn01";
    GGO_LOG_ERROR(logger) << "logger error01";
    GGO_LOG_FATAL(logger) << "logger fatal01";

    logger->setLevel(GGo::LogLevel::WARN);
    std::cout << "logger_level : " << "WARN" << std::endl;
    std::cout << "file_level: " << "ERROR" << std::endl;
    GGO_LOG_DEBUG(logger) << "logger debug02";
    GGO_LOG_INFO(logger) << "logger info02";
    GGO_LOG_WARN(logger) << "logger warn02";
    GGO_LOG_ERROR(logger) << "logger error02";
    GGO_LOG_FATAL(logger) << "logger fatal02";
    logger->setLevel(GGo::LogLevel::FATAL);
    std::cout << "logger_level : " << "FATAL" << std::endl;
    std::cout << "file_level: " << "ERROR" << std::endl;
    GGO_LOG_DEBUG(logger) << "logger debug03";
    GGO_LOG_INFO(logger) << "logger info03";
    GGO_LOG_WARN(logger) << "logger warn03";
    GGO_LOG_ERROR(logger) << "logger error03";
    GGO_LOG_FATAL(logger) << "logger fatal03";

}