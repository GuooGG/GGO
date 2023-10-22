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

void showBasicType(){
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
    } // int 类型配置
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
}


//测试自定义类型与stl类型组合解析
//自定义类型
class Person{
public:
    Person(){

    };
    std::string m_name;
    int m_age = 0;
    bool m_sex = 0;;
    std::string toString() const{
        std::stringstream ss;
        ss << "[Person name=" << m_name
           << "age=" << m_age
           << "sex=" << m_sex
           << "]";
        return ss.str();
    }
    bool operator==(const Person& oth) const
    {
        return m_name == oth.m_name
            && m_age == oth.m_age
            && m_sex == oth.m_sex;
    }
};
//模板偏特化
namespace GGo{

template<>
class LexicalCast<std::string,Person>{
public:
    Person operator()(const std::string& ymlstr){
        YAML::Node node = YAML::Load(ymlstr);
        Person p;
        p.m_name = node["name"].as<std::string>();
        p.m_age = node["age"].as<int>();
        p.m_sex = node["sex"].as<bool>();
        return p;
    }

};

template <>
class LexicalCast<Person, std::string>
{
public:
    std::string operator()(const Person &p)
    {
        YAML::Node node;
        node["name"] = p.m_name;
        node["age"] = p.m_age;
        node["sex"] = p.m_sex;
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};
}
GGo::ConfigVar<Person>::ptr g_person = 
    GGo::Config::Lookup("class.person",Person(),"class person");
GGo::ConfigVar<std::map<std::string, Person>>::ptr g_person_map =
    GGo::Config::Lookup("class.map", std::map<std::string, Person>(), "system person");
GGo::ConfigVar<std::map<std::string, std::vector<Person>>>::ptr g_person_vecmap =
    GGo::Config::Lookup("class.vecmap", std::map<std::string, std::vector<Person>>(), "system person");
void showCustomType(){
    GGO_LOG_INFO(GGO_LOG_ROOT())  << g_person->getValue().toString() << std::endl << g_person->toString();
    auto m = g_person_map->getValue();
    for (auto &i : m)
    {
        GGO_LOG_INFO(GGO_LOG_ROOT()) << i.first << " - " << i.second.toString();
    }
    auto v = g_person_vecmap->getValue();
    for(auto& kv:v){
        GGO_LOG_INFO(GGO_LOG_ROOT()) << kv.first <<"  " << kv.second[0].toString() << kv.second[1].toString();
    }
}



int main()
{
    g_person->addListener([](const Person& oldv,const Person& newv){
        GGO_LOG_INFO(GGO_LOG_ROOT()) << "callback called";
    });
    showCustomType();
    //载入配置
    YAML::Node node = YAML::LoadFile("/root/workspace/GGoSeverFrame/Test/conf/log.yml");
    GGo::Config::loadFromYaml(node);
    std::cout<<"=========================================================================================================================================="<<std::endl;
   showCustomType();
    return 0;
}