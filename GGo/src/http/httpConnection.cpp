#include "http/httpConnection.h"


namespace GGo{ 
namespace HTTP{


std::string HTTPResult::toString() const
{
    std::stringstream ss;
    ss << "[HTTPResult result=" << result
        << " error=" << error
        << " response=" << (response ? response->toString() : "nullptr")
        << "]";
    return ss.str();
}


}
}

