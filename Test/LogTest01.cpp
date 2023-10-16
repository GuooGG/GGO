#include"LogSystem.h"

int main(){
    static std::shared_ptr<GGo::Logger> g_logger = GGO_LOG_ROOT();
    GGo::LogEvent*e = new GGo::LogEvent(g_logger,GGo::LogLevel::FATAL,__FILE__,__LINE__,0,0,0,time(0),"");
    e->format("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%L%T%m%n");
    std::shared_ptr<GGo::LogEvent> pe(e);
    g_logger->fatal(pe);
    return 0;
}