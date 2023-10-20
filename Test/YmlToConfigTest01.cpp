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
    GGo::ConfigVar<std::vector<int> >::ptr intvec1_val = GGo::Config::Lookup("system.intvec1",std::vector<int>{1,2},"intvec1");
    GGo::ConfigVar<std::vector<int> >::ptr intvec2_val = GGo::Config::Lookup("system.intvec2", std::vector<int>{3,4}, "intvec1");
    YAML::Node node = YAML::LoadFile("/root/workspace/GGoSeverFrame/Test/conf/log.yml");
    // std::cout << "show root:" << std::endl;
    // std::cout << node << std::endl;
    std::list<std::pair<std::string, const YAML::Node>> all_nodes;
    std::cout << "============================" << std::endl;
    listAllMembers("",node,all_nodes);
    for(auto& pair : all_nodes){
        std::cout  << pair.first <<std::endl;
    }
    auto vec1 = intvec1_val->getValue();
    auto vec2 = intvec2_val->getValue();
    for(int& i :vec1){
        std::cout << i << std::endl;
    }
    for (int &i : vec2)
    {
        std::cout << i << std::endl;
    }
    std::cout << "============================" << std::endl;
    GGO_LOG_INFO(GGO_LOG_ROOT()) << g_int_value_config->getValue();
    GGo::Config::loadFromYaml(node);
    GGO_LOG_INFO(GGO_LOG_ROOT()) << g_int_value_config->getValue();
    std::cout << "============================" << std::endl;
    auto map = GGo::Config::GetDatas();
    for(auto it = node.begin(); it != node.end(); it++){
        std::cout << it->first << std::endl;
    }
    vec1 = intvec1_val->getValue();
    vec2 = intvec2_val->getValue();
    for (int &i : vec1)
    {
        std::cout << "intvec1 after" << i << std::endl;
    }
    for (int &i : vec2)
    {
        std::cout << i << std::endl;
    }
    return 0;
}