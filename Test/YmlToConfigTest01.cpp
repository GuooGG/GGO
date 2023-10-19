#include"config.h"
#include"LogSystem.h"
#include<iostream>
#include<yaml-cpp/yaml.h>
#include<fstream>
void listAllMembers(const std::string &prefix,
                    const YAML::Node node,
                    std::list<std::pair<std::string, const YAML::Node>> &output)
{
    if (prefix.find_first_not_of("avcdefghijklmnopqrstuvwxyz._0123456789") != std::string::npos)
    { // 日志项目名非法
        GGO_LOG_ERROR(GGO_LOG_ROOT()) << "Config invalid name: "
                                      << prefix << " . " << node;
        return;
    }
    output.emplace_back(std::make_pair(prefix, node));
    if (node.IsMap())
    {
        for (auto it = node.begin(); it != node.end(); it++)
        {
            listAllMembers(prefix.empty() ? it->first.Scalar() : prefix + "." + it->first.Scalar(), it->second, output);
        }
    }
}
int main()
{
    GGo::ConfigVar<int>::ptr g_int_value_config = GGo::Config::Lookup("system.port", (int)8080, "system port");
    YAML::Node node = YAML::LoadFile("/root/workspace/GGoSeverFrame/conf/log.yml");
    // std::cout << "show root:" << std::endl;
    // std::cout << node << std::endl;
    std::list<std::pair<std::string, const YAML::Node>> all_nodes;
    std::cout << "============================" << std::endl;
    listAllMembers("",node,all_nodes);
    // for(auto& pair : all_nodes){
    //     std::cout << "path:[" << pair.first << "]  :  val: ["  << pair.second << " ] "<<std::endl;
    // }
    // std::cout << "============================" << std::endl;
    GGO_LOG_INFO(GGO_LOG_ROOT()) << g_int_value_config->getValue();
    GGo::Config::loadFromYaml(node);
    GGO_LOG_INFO(GGO_LOG_ROOT()) << g_int_value_config->getValue();
    std::cout << "============================" << std::endl;
    auto map = GGo::Config::GetDatas();
    for(auto& kv : map){
        std::cout << kv.first << std::endl;
    }
    return 0;
}