#include<iostream>
#include"LogSystem.h"

int main(int argc,char** argv){
    //创建日志器
    GGo::Logger::ptr logger(new GGo::Logger);
    //添加控制台日志目标
    logger->addAppender(GGo::LogAppender::ptr(new GGo::StdoutLogAppender));
    //添加文件输出日志目标
    GGo::FileLogAppender::ptr file_appender(new GGo::FileLogAppender("./log.txt"));
    //设置新的格式器格式
    GGo::LogFormatter::ptr fmt(new GGo::LogFormatter("%d%T%p%T%m%n"));
    //给文件输出日志设定新的输出格式
    file_appender->setFormatter(fmt);
    //给文件输出日志设定日志级别，低于ERROR不输出
    file_appender->setLevel(GGo::LogLevel::ERROR);

    logger->addAppender(file_appender);

    std::cout<<"hello GGo log"<<std::endl;

    GGo::LogEvent::ptr debug(new GGo::LogEvent(logger,GGo::LogLevel::DEBUG,__FILE__,__LINE__,0,GGo::GetThreadID(),GGo::GetThreadID(),time(0),"ThreadName"));
    GGo::LogEvent::ptr error(new GGo::LogEvent(logger, GGo::LogLevel::ERROR, __FILE__, __LINE__, 0, GGo::GetThreadID(), GGo::GetThreadID(), time(0), "ThreadName"));
    GGo::LogEvent::ptr fatal(new GGo::LogEvent(logger, GGo::LogLevel::FATAL, __FILE__, __LINE__, 0, GGo::GetThreadID(), GGo::GetThreadID(), time(0), "ThreadName"));

    logger->log(debug->getLevel(),debug);
    logger->log(error->getLevel(),error);
    logger->log(fatal->getLevel(),fatal);
}