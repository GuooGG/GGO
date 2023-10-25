#include "config.h"

namespace GGo{


void listAllMembers(const std::string& prefix,
                          const YAML::Node node,
                          std::list<std::pair<std::string,const YAML::Node> >& output)
{
    if(prefix.find_first_not_of("avcdefghijklmnopqrstuvwxyz._0123456789") != std::string::npos)
    {   //日志项目名非法
        GGO_LOG_ERROR(GGO_LOG_ROOT()) << "Config invalid name: " 
                                      << prefix << " . " << node;
        return;
    }
    output.emplace_back(std::make_pair(prefix,node));
    if(node.IsMap()){
        for(auto it = node.begin(); it != node.end(); it++){
            listAllMembers(prefix.empty() ? it->first.Scalar() : prefix + "." + it->first.Scalar(), it->second, output);
        }
    }

}

void Config::loadFromYaml(YAML::Node& root)
{
    std::list<std::pair<std::string, const YAML::Node> > all_nodes;
    listAllMembers("", root, all_nodes);
    for(auto& i : all_nodes){
        std::string key = i.first;
        if(key.empty()){
            continue;
        }
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        ConfigVarBase::ptr var = lookupBase(key);
        if(var){
            if(i.second.IsScalar()){
                var->fromString(i.second.Scalar());
            }else{
                std::stringstream ss;
                ss << i.second;
                var->fromString(ss.str());
            }
        }

    }
}

ConfigVarBase::ptr Config::lookupBase(const std::string &name)
{
    RWMutexType::readLock lock(GetMutex());
    auto it = GetDatas().find(name);
    if(it == GetDatas().end()){
        return nullptr;
    }
    return it->second;

}
}