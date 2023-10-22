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

// 配置系统整合日志系统 ，定义 LogAppender 和 logger 的定义结构体，并作模板偏特化

static const char *LogLevelTOString(GGo::LogLevel level)
{
    switch (level)
    {
    case GGo::LogLevel::DEBUG:
        return "DEBUG";
        break;
    case GGo::LogLevel::INFO:
        return "INFO";
        break;
    case GGo::LogLevel::WARN:
        return "WARN";
        break;
    case GGo::LogLevel::ERROR:
        return "ERROR";
        break;
    case GGo::LogLevel::FATAL:
        return "FATAL";
    default:
        return "UNKNOWN";
    }
}
static GGo::LogLevel FromStringToLogLevel(const std::string &str)
{
    if (str == "DEBUG" || str == "debug")
    {
        return GGo::LogLevel::DEBUG;
    }
    if (str == "INFO" || str == "info")
    {
        return GGo::LogLevel::INFO;
    }
    if (str == "WARN" || str == "warn")
    {
        return GGo::LogLevel::WARN;
    }
    if (str == "ERROR" || str == "error")
    {
        return GGo::LogLevel::ERROR;
    }
    if (str == "FATAL" || str == "fatal")
    {
        return GGo::LogLevel::FATAL;
    }
    return GGo::LogLevel::UNKNOWN;
}
struct LogAppenderDefine
{
    /// @brief 1 file / 2 stdout
    int type = 0;
    GGo::LogLevel level = GGo::LogLevel::UNKNOWN;
    std::string formatter;
    std::string file;

    bool operator==(const LogAppenderDefine &other) const
    {
        return type == other.type &&
               level == other.level &&
               formatter == other.formatter &&
               file == other.file;
    }
};
struct LogDefine
{
    std::string name;
    GGo::LogLevel level = GGo::LogLevel::UNKNOWN;
    std::string formatter;
    std::vector<LogAppenderDefine> appenders;

    bool operator==(const LogDefine &other) const
    {
        return name == other.name &&
               level == other.level &&
               formatter == other.formatter &&
               appenders == other.appenders;
    }
    // 为了能使用std::map，需要提供大小比较的方法
    bool operator<(const LogDefine &other) const
    {
        return name < other.name;
    }
    bool isValid() const
    {
        return !name.empty();
    }
};
namespace GGo{
/// @brief 模板类型偏特化 实现 YAML 字符串和LogDefine 自定义类的相互转换
template <>
class LexicalCast<std::string, LogDefine>
{
public:
    LogDefine operator()(const std::string &ymlstr)
    {
        YAML::Node node = YAML::Load(ymlstr);
        LogDefine ret;
        if (!node["name"].IsDefined())
        {
            std::cout << "log config error: name is a null, " << node << std::endl;
            throw std::logic_error("log config is null");
        }
        if (!node["level"].IsDefined())
        {
            std::cout << "log config error: level is a null, " << node << std::endl;
            throw std::logic_error("log config is null");
        }
        ret.name = node["name"].as<std::string>();
        ret.level = FromStringToLogLevel(node["level"].as<std::string>());
        if (node["formatter"].IsDefined())
        {
            ret.formatter = node["formatter"].as<std::string>();
        }
        if (node["appenders"].IsDefined())
        {
            for (size_t i = 0; i < node["appenders"].size(); i++)
            {
                YAML::Node appender_node = node["appenders"][i];
                if (!appender_node["type"].IsDefined())
                {
                    std::cout << "log config error: appender type is null" << std::endl;
                }
                std::string type = appender_node["type"].as<std::string>();
                LogAppenderDefine appender_define;
                if (type == "FileLogAppender")
                {
                    appender_define.type = 1;
                    if (appender_node["level"].IsDefined())
                    {
                        appender_define.level = FromStringToLogLevel(appender_node["level"].as<std::string>());
                    }
                    if (!appender_node["file"].IsDefined())
                    {
                        std::cout << "log config error: fileappender file is null"
                                  << appender_node << std::endl;
                        continue;
                    }
                    appender_define.file = appender_node["file"].as<std::string>();
                    if (appender_node["formatter"].IsDefined())
                    {
                        appender_define.formatter = appender_node["formatter"].as<std::string>();
                    }
                }
                else if (type == "StdoutLogAppender")
                {
                    appender_define.type = 2;
                    if (appender_node["level"].IsDefined())
                    {
                        appender_define.level = FromStringToLogLevel(appender_node["level"].as<std::string>());
                    }
                    if (appender_node["formatter"].IsDefined())
                    {
                        appender_define.formatter = appender_node["formatter"].as<std::string>();
                    }
                }
                else
                {
                    std::cout << "log config error: appender type is valid" << std::endl;
                }
                ret.appenders.push_back(appender_define);
            }
        }
        return ret;
    }
};

template <>
class LexicalCast<LogDefine, std::string>
{
public:
    std::string operator()(const LogDefine &input)
    {
        YAML::Node node;
        std::stringstream ss;
        node["name"] = input.name;
        if (input.level != LogLevel::UNKNOWN)
        {
            node["level"] = LogLevelTOString(input.level);
        }
        if (!input.formatter.empty())
        {
            node["formatter"] = input.formatter;
        }

        for (auto &appender : input.appenders)
        {
            YAML::Node appender_node;
            if (appender.type == 1)
            {
                appender_node["type"] = "FileLogAppender";
                appender_node["file"] = appender.file;
            }
            else if (appender.type == 2)
            {
                appender_node["type"] = "StdoutLogAppender";
            }
            if (appender.level != LogLevel::UNKNOWN)
            {
                appender_node["level"] = LogLevelTOString(appender.level);
            }
            if (!appender.formatter.empty())
            {
                appender_node["formatter"] = appender.formatter;
            }

            node["appenders"].push_back(appender_node);
        }
        ss << node;
        return ss.str();
    }
};
}

GGo::ConfigVar<std::vector<LogDefine>>::ptr g_log_vec_config =
    GGo::Config::Lookup("logs",std::vector<LogDefine>(), "logs config");

void test_log_config(){
    auto logdefines = g_log_vec_config->getValue();
    for(auto& logdefine : logdefines){
        GGO_LOG_INFO(GGO_LOG_ROOT()) << logdefine.name;
        GGO_LOG_INFO(GGO_LOG_ROOT()) << LogLevelTOString(logdefine.level);
        GGO_LOG_INFO(GGO_LOG_ROOT()) << logdefine.formatter;
        auto appenders = logdefine.appenders;
        for(auto appender : appenders){
            GGO_LOG_INFO(GGO_LOG_ROOT()) << appender.type;
            GGO_LOG_INFO(GGO_LOG_ROOT()) << LogLevelTOString(appender.level);
            GGO_LOG_INFO(GGO_LOG_ROOT()) << appender.file;
            GGO_LOG_INFO(GGO_LOG_ROOT()) << appender.formatter;
        }
    }

}

int main()
{

    // 测试回调函数
    // g_person->addListener([](const Person& oldv,const Person& newv){
    //     GGO_LOG_INFO(GGO_LOG_ROOT()) << "callback called";
    // });
    test_log_config();
    //载入配置
    YAML::Node node = YAML::LoadFile("/root/workspace/GGoSeverFrame/Test/conf/log.yml");
    GGo::Config::loadFromYaml(node);
    std::cout<<"================================================================================================================================"<<std::endl;
    test_log_config();
    return 0;
}