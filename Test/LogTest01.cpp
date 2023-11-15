#include "GGo.h"
int main(){
    static std::shared_ptr<GGo::Logger> g_logger = GGO_LOG_ROOT();
    GGo::LogEvent*e = new GGo::LogEvent(g_logger,GGo::LogLevel::FATAL,__FILE__,__LINE__,GGo::GetThreadID(),GGo::GetFiberID(),0,time(0),"");
    std::shared_ptr<GGo::LogEvent> pe(e);
    pe->getSS() << "hello";
    g_logger->fatal(pe);
    GGO_LOG_LEVEL(g_logger,GGo::LogLevel::DEBUG) << "ll";
    return 0;

}