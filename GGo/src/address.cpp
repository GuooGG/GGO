#include"address.h"
#include"logSystem.h"
#include"endianParser.h"

#include<sstream>

namespace GGo{

static GGo::Logger::ptr g_logger = GGO_LOG_NAME("system");


int Address::getFamily() const
{
    return getAddr()->sa_family;
}

std::string Address::toString() const
{
    std::stringstream ss;
    insert(ss);
    return ss.str();


}


}

