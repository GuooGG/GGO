/**
 * @file HTTPSever.cpp
 * @author GGO
 * @brief HTTP服务器DEMO
 * @version 0.1
 * @date 2024-02-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "GGo.h"

GGo::Logger::ptr g_logger = GGO_LOG_ROOT();
GGo::IOScheduler::ptr worker;

void run(){
    g_logger->setLevel(GGo::LogLevel::INFO);
    GGo::Address::ptr addr = GGo::Address::LookupAnyIPAddress("0.0.0.0:1145");
    if(!addr){
        GGO_LOG_ERROR(g_logger) << "get address error";
        return;
    }
    
    GGo::HTTP::HTTPSever::prt http_sever(new GGo::HTTP::HTTPSever(true, worker.get()));

    bool ssl = false;

    while(!http_sever->bind(addr, ssl)){
        GGO_LOG_ERROR(g_logger) << "bind " << addr << " fail";
        sleep(1);
    }
    if(ssl){

    }

    http_sever->start();
}

int main(int argc, char** argv){
    GGo::IOScheduler iom(1);
    worker.reset(new GGo::IOScheduler(4, false));
    iom.schedule(run);
    return 0;
}