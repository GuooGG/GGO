#include"config.h"
#include"LogSystem.h"
#include<iostream>
#include<yaml-cpp/yaml.h>
#include<fstream>

//约定配置项默认值
GGo::ConfigVar<int>::ptr g_int_value_config = 
    GGo::Config::Lookup("system.port", (int)8080, "system.port");
GGo::ConfigVar<float>::ptr g_float_value_config = 
    GGo::Config::Lookup("system.value", (float)100.2f, "system.value");
GGo::ConfigVar<std::vector<int>>::ptr g_int_vec_value_config =
    GGo::Config::Lookup("system.intvec", std::vector<int>{7,9}, "system.intvec");
GGo::ConfigVar<std::list<std::string>>::ptr g_str_list_value_config = 
    GGo::Config::Lookup("system.strlist",std::list<std::string>{"h","w"},"system.strlist");
GGo::ConfigVar<std::set<int>>::ptr g_int_set_value_config = 
    GGo::Config::Lookup("system.intset", std::set<int>{6,6,8},"system.intset");
GGo::ConfigVar<std::unordered_set<int> >::ptr g_int_uset_value_config = 
    GGo::Config::Lookup("system.intuset",std::unordered_set<int>{4,6},"system.intuset");
GGo::ConfigVar<std::map<std::string,int> >::ptr g_str_int_map_value_config = 
    GGo::Config::Lookup("system.strintmap", std::map<std::string,int>{{"k",2}}, "system.strintmap");
GGo::ConfigVar<std::unordered_map<std::string, int>>::ptr g_str_int_umap_value_config =
    GGo::Config::Lookup("system.strintumap", std::unordered_map<std::string, int>{{"g", 3}}, "system.strintumap");

int main()
{
    //int 类型配置
    GGO_LOG_INFO(GGO_LOG_ROOT()) << "g_int_value_config before: " << g_int_value_config->getValue();
    //float 类型配置
    GGO_LOG_INFO(GGO_LOG_ROOT()) << "g_float_value_config before: " << g_float_value_config->getValue();
    //vec 类型配置
    GGO_LOG_INFO(GGO_LOG_ROOT()) << "g_int_vec_value_config before: ";
    for(auto& i : g_int_vec_value_config->getValue()){
        GGO_LOG_INFO(GGO_LOG_ROOT()) << i;
    }
    //list 类型配置
    GGO_LOG_INFO(GGO_LOG_ROOT()) << "g_str_list_value_config before: ";
    for(auto& i : g_str_list_value_config->getValue()){
        GGO_LOG_INFO(GGO_LOG_ROOT()) << i;
    }
    //set 类型配置
    GGO_LOG_INFO(GGO_LOG_ROOT()) << "g_int_set_value_config before: ";
    for(auto& i : g_int_set_value_config->getValue()){
        GGO_LOG_INFO(GGO_LOG_ROOT()) << i;
    }
    // unordered_set 类型配置
    GGO_LOG_INFO(GGO_LOG_ROOT()) << "g_int_uset_value_config before: ";
    for (auto &i : g_int_uset_value_config->getValue())
    {
        GGO_LOG_INFO(GGO_LOG_ROOT()) << i;
    }
    // map类型配置
    GGO_LOG_INFO(GGO_LOG_ROOT()) << "g_str_int_map_value_config before: ";
    for(auto& i :g_str_int_map_value_config->getValue()){
        GGO_LOG_INFO(GGO_LOG_ROOT()) << i.first << " : " << i.second;
    }
    // unordered_map类型配置
    GGO_LOG_INFO(GGO_LOG_ROOT()) << "g_str_uint_umap_value_config before: ";
    for (auto &i : g_str_int_umap_value_config->getValue())
    {
        GGO_LOG_INFO(GGO_LOG_ROOT()) << i.first << " : " << i.second;
    }

    //载入配置
    YAML::Node node = YAML::LoadFile("/root/workspace/GGoSeverFrame/Test/conf/log.yml");
    GGo::Config::loadFromYaml(node);
    std::cout<<"===================================================================================================================================================="<<std::endl;
    // int 类型配置
    GGO_LOG_INFO(GGO_LOG_ROOT()) << "g_int_value_config after: " << g_int_value_config->getValue();
    // float 类型配置
    GGO_LOG_INFO(GGO_LOG_ROOT()) << "g_float_value_config after: " << g_float_value_config->getValue();
    // vec 类型配置
    GGO_LOG_INFO(GGO_LOG_ROOT()) << "g_int_vec_value_config after: ";
    for (auto &i : g_int_vec_value_config->getValue())
    {
        GGO_LOG_INFO(GGO_LOG_ROOT()) << i;
    }
    // list 类型配置
    GGO_LOG_INFO(GGO_LOG_ROOT()) << "g_str_list_value_config after: ";
    for (auto &i : g_str_list_value_config->getValue())
    {
        GGO_LOG_INFO(GGO_LOG_ROOT()) << i;
    }
    // set 类型配置
    GGO_LOG_INFO(GGO_LOG_ROOT()) << "g_int_set_value_config after: ";
    for (auto &i : g_int_set_value_config->getValue())
    {
        GGO_LOG_INFO(GGO_LOG_ROOT()) << i;
    }
    // unordered_set 类型配置
    GGO_LOG_INFO(GGO_LOG_ROOT()) << "g_int_uset_value_config after: ";
    for (auto &i : g_int_uset_value_config->getValue())
    {
        GGO_LOG_INFO(GGO_LOG_ROOT()) << i;
    }
    // map类型配置
    GGO_LOG_INFO(GGO_LOG_ROOT()) << "g_str_int_map_value_config after: ";
    for (auto &i : g_str_int_map_value_config->getValue())
    {
        GGO_LOG_INFO(GGO_LOG_ROOT()) << i.first << " : " << i.second;
    }
    // unordered_map类型配置
    GGO_LOG_INFO(GGO_LOG_ROOT()) << "g_str_uint_umap_value_config after: ";
    for (auto &i : g_str_int_umap_value_config->getValue())
    {
        GGO_LOG_INFO(GGO_LOG_ROOT()) << i.first << " : " << i.second;
    }

    return 0;
}