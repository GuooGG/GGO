/**
 * @file config.h
 * @author GGo
 * @brief 配置模块
 * @date 2023-10-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include<memory>
#include<sstream>
#include<string>
#include<unordered_map>
#include<vector>
#include<functional>
#include<list>
#include<map>
#include<set>
#include<unordered_set>
#include<functional>
#include<yaml-cpp/yaml.h>
#include<boost/lexical_cast.hpp>
#include"logSystem.h"



namespace GGo{


class ConfigVarBase{
public:
    using ptr = std::shared_ptr<ConfigVarBase>;

    ConfigVarBase(const std::string& name,const std::string& description = "")
        :m_name(name)
        ,m_description(description)
    {
        //配置文件项目名全部小写
        std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
    }

    virtual ~ConfigVarBase(){}

    virtual std::string toString() = 0;
    virtual bool fromString(const std::string& val) = 0;

    const std::string& getName() const{return m_name;}
    const std::string& getDescription() const {return m_description;}


protected:
    std::string m_name;
    std::string m_description;
};


/// @brief 类型转换默认类型 用于基础类型之间的转换
/// @tparam F 原类型
/// @tparam T 目标类型
template<class F,class T>
class LexicalCast{
public:
    T operator()(const F& f){
        return boost::lexical_cast<T>(f);
    }
};

/// @brief 模板类型偏特化，从 YAML 字符串转为 vector<T>
template<class T>
class LexicalCast<std::string, std::vector<T> >{
public:
    std::vector<T> operator()(const std::string& ymlstr){
        YAML::Node node = YAML::Load(ymlstr);
        typename std::vector<T> ret;
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); i++){
            ss.str("");
            ss << node[i];
            ret.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return ret;
    }

};

/// @brief 模板类型偏特化，从 vector<T> 转为 YAML 字符串
template<class T>
class LexicalCast<std::vector<T>, std::string>{
public:
    std::string operator()(const std::vector<T>& vec){
        YAML::Node node(YAML::NodeType::Sequence);
        std::stringstream ss;
        for(auto& i : vec){
            node.push_back(YAML::Load(LexicalCast<T,std::string>()(i)));
            ss << node;
        }
        ss << node;
        return ss.str();
    }
};

/// @brief 模板类型偏特化，从 TAML 字符串转为 list<T>
template<class T>
class LexicalCast<std::string, std::list<T> >{
public:
    std::list<T> operator()(const std::string& ymlstr){
        YAML::Node node = YAML::Load(ymlstr);
        typename std::list<T> listT;
        std::stringstream ss;
        for(size_t i = 0; i != node.size(); i++){
           ss.str("");
           ss << node[i];
            listT.push_back(LexicalCast<std::string,T>()(ss.str()));
        }
        return listT;
    }
};

/// @brief 模板类型偏特化，从 list<T> 转为 YAML 字符串
template <class T>
class LexicalCast<std::list<T>, std::string>{
public:
    std::string operator()(const std::list<T>& listT){
        std::stringstream ss;
        YAML::Node node(YAML::NodeType::Sequence);
        for(auto& t: listT){
            node.push_back(YAML::Load(LexicalCast<T,std::string>()(t)));
        }
        ss << node;
        return ss.str();
    }
};

/// @brief 模板类型偏特化，从 TAML 字符串转为 set<T>
template <class T>
class LexicalCast<std::string, std::set<T>>
{
public:
    std::set<T> operator()(const std::string &ymlstr)
    {
        YAML::Node node = YAML::Load(ymlstr);
        typename std::set<T> setT;
        std::stringstream ss;
        for (size_t i = 0; i != node.size(); i++)
        {
            ss.str("");
            ss << node[i];
            setT.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return setT;
    }
};

/// @brief 模板类型偏特化，从 set<T> 转为 YAML 字符串
template <class T>
class LexicalCast<std::set<T>, std::string>
{
public:
    std::string operator()(const std::set<T> &setT)
    {
        std::stringstream ss;
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto &t : setT)
        {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(t)));
        }
        ss << node;
        return ss.str();
    }
};

/// @brief 模板类型偏特化，从 TAML 字符串转为 unordered_set<T>
template <class T>
class LexicalCast<std::string, std::unordered_set<T>>
{
public:
    std::unordered_set<T> operator()(const std::string &ymlstr)
    {
        YAML::Node node = YAML::Load(ymlstr);
        typename std::unordered_set<T> setT;
        std::stringstream ss;
        for (size_t i = 0; i != node.size(); i++)
        {
            ss.str("");
            ss << node[i];
            setT.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return setT;
    }
};

/// @brief 模板类型偏特化，从 unordered_set<T> 转为 YAML 字符串
template <class T>
class LexicalCast<std::unordered_set<T>, std::string>
{
public:
    std::string operator()(const std::unordered_set<T> &setT)
    {
        std::stringstream ss;
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto &t : setT)
        {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(t)));
        }
        ss << node;
        return ss.str();
    }
};

/// @brief 模板类型偏特化，从 map<string,T> 转为 YAML 字符串 
template<class T>
class LexicalCast<std::map<std::string,T>, std::string>{
public:
    std::string operator()(const std::map<std::string, T>& mapT){
        YAML::Node node(YAML::NodeType::Map);
        std::stringstream ss;
        for (auto &kv : mapT)
        {
            node[kv.first] = YAML::Load(LexicalCast<T,std::string>()(kv.second));
        }
        ss << node;
        return ss.str();
    }
};

/// @brief 模板类型偏特化，从 YAML 字符串转为 map<string,T>
template <class T>
class LexicalCast<std::string, std::map<std::string, T> >{
public:
    std::map<std::string, T> operator()(const std::string& ymlstr){
        YAML::Node node = YAML::Load(ymlstr);
        typename std::map<std::string,T> mapT;
        std::stringstream ss;
        for(auto it = node.begin(); it != node.end(); it++){
            ss.str("");
            ss << it->second;
            mapT.insert(std::make_pair(it->first.Scalar(),
                        LexicalCast<std::string,T>()(ss.str())));
        }
        return mapT;
    }
};

/// @brief 模板类型偏特化，从 unordered_map<string,T> 转为 YAML 字符串
template <class T>
class LexicalCast<std::unordered_map<std::string, T>, std::string>
{
public:
    std::string operator()(const std::unordered_map<std::string, T> &mapT)
    {
        YAML::Node node(YAML::NodeType::Map);
        std::stringstream ss;
        for (auto &kv : mapT)
        {
            node[kv.first] = YAML::Load(LexicalCast<T, std::string>()(kv.second));
        }
        ss << node;
        return ss.str();
    }
};

/// @brief 模板类型偏特化，从 YAML 字符串转为 unordered_map<string,T>
template <class T>
class LexicalCast<std::string, std::unordered_map<std::string, T>>
{
public:
    std::unordered_map<std::string, T> operator()(const std::string &ymlstr)
    {
        YAML::Node node = YAML::Load(ymlstr);
        typename std::unordered_map<std::string, T> mapT;
        std::stringstream ss;
        for (auto it = node.begin(); it != node.end(); it++)
        {
            ss.str("");
            ss << it->second;
            mapT.insert(std::make_pair(it->first.Scalar(),
                                       LexicalCast<std::string, T>()(ss.str())));
        }
        return mapT;
    }
};


/// @brief 配置量类型
/// @tparam T 配置量的值类型
/// @tparam FromStr 用于从字符串转为值类型的类型
/// @tparam ToStr 用于从值类型转为字符串的类型
template<class T ,class FromStr = LexicalCast< std::string, T>
                 ,class ToStr = LexicalCast< T, std::string> >
class ConfigVar : public ConfigVarBase{

public:
    using ptr = std::shared_ptr<ConfigVar>;
    using RWMutexType = RWMutex;
    /// @brief 配置项的值发生改变时触发的回调函数
    using on_change_callback = std::function<void (const T& oldv,const T& newv)>;

    ConfigVar(const std::string& name,const T& default_value
            ,const std::string& description)
            :ConfigVarBase(name,description)
            ,m_val(default_value)
            {}
    std::string toString() override{
        try{
            RWMutexType::readLock lock(m_mutex);
            return ToStr()(m_val);
        }catch (std::exception& e){
            GGO_LOG_ERROR(GGO_LOG_ROOT()) << "ConfigVar::toString exception "
                << e.what() <<" convert " << "XXX" << "to string"
                << "name=" << m_name;
        }
        return "";
    }
    bool fromString(const std::string& val) override{
        try{

            setValue(FromStr()(val));
        }catch(std::exception& e){
            GGO_LOG_ERROR(GGO_LOG_ROOT()) << "ConfigVar::fromstring exception " 
                                          << e.what() << " convert string to " << typeid(T).name()
                                          << " name=" << m_name
                                          << " - " << val;
        }
        return false;
    }

    /// @brief 设定新的配置项值，执行委托的函数
    void setValue(const T& v){
        {
            RWMutexType::readLock lock(m_mutex);
            if (m_val == v)
            {
                return;
            }
            for (auto &kv : m_cbs)
            {
                kv.second(m_val, v);
            }
        }
        RWMutexType::writeLock lock(m_mutex);
        m_val = v;
    }

    const T getValue() {
        RWMutexType::readLock lock(m_mutex);
        return m_val;
    }

    /// @brief 添加对应的回调函数委托
    /// @param cb 回调函数指针
    /// @return 回调函数对应的唯一key，用于定位对应的回调函数
    uint64_t addListener(on_change_callback cb){
        static uint64_t s_cbfunc_id = 0;
        RWMutexType::writeLock lock(m_mutex);
        m_cbs[s_cbfunc_id] = cb;
        s_cbfunc_id++;
        return s_cbfunc_id;
    }

    /// @brief 得到对应key的回调函数委托
    on_change_callback getListener(uint64_t key){
        RWMutexType::readLock lock(m_mutex);
        auto it = m_cbs.find(key);
        if(it == m_cbs.end()){
            return nullptr;
        }
        return it->second;
    }

    /// @brief  清理对应key的回调函数委托
    void delListener(uint64_t key){
        RWMutexType::writeLock lock(m_mutex);
        m_cbs.erase(key);
    }

    /// @brief 清理所有的回调函数委托
    void clearListener(){
        RWMutexType::writeLock lock(m_mutex);
        m_cbs.clear();
    }
private:
    T m_val;
    /// @brief 委托模式，存储回调函数映射表
    std::map<uint64_t,on_change_callback> m_cbs;
    // mutex
    RWMutexType m_mutex;
};

class Config{
public:
    using ptr = std::shared_ptr<Config>;
    using ConfigVarMap = std::unordered_map<std::string,ConfigVarBase::ptr>;
    using RWMutexType = RWMutex;

    /// @brief 获取/创建对应参数名的配置参数
    /// @param name 配置参数名称
    /// @param default_value 参数默认值
    /// @param description 参数描述
    /// @details 获取参数名为name的配置参数，如果存在直接返回
    ///          如果不存在，则创建该配置参数并用default_value赋值
    /// @return 返回对应的配置，如果参数名存在但是类型不匹配则返回nulptr
    /// @exception 如果参数名包含非法字符[^0-9a-z_.] 则抛出异常 std::valid_argument
    template<class T>
    static typename ConfigVar<T>::ptr Lookup(
        const std::string& name,
        const T& default_value,
        const std::string& description = "")
{       
        RWMutexType::writeLock lock(GetMutex());
        auto it = GetDatas().find(name);
        if(it != GetDatas().end()){
            auto tmp = std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
            if(tmp){
                GGO_LOG_INFO(GGO_LOG_ROOT()) << "Lookup name=" << name << " exists";
                return tmp; 
            }else{
                GGO_LOG_ERROR(GGO_LOG_ROOT()) << "Lookup name=" << name << " exists but type not "  << typeToName<T>();
                return nullptr;
            }
        }
        //数据中没有name配置参数,检查名称是否合法
        if(name.find_first_not_of("abcdefghikjlmnopqrstuvwxyz._012345678") != std::string::npos){
            GGO_LOG_ERROR(GGO_LOG_ROOT()) << "Lookup name invalid :" << name;
            throw std::invalid_argument(name);
        }
        //名称合法，可以创建新参数
        typename ConfigVar<T>::ptr newConfigVal(new ConfigVar<T>(name,default_value,description));
        GetDatas()[name] = newConfigVal;
        return newConfigVal;

    }

    /// @brief 查找配置参数
    /// @param name 配置参数名称
    /// @return 返回配置参数名为name的配置参数 
    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name){
        RWMutexType::readLock lock(GetMutex());
        auto it = GetDatas().find(name);
        if(it == GetDatas().end()){
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
    }

    /// @brief 读入yaml节点 将其转换为配置名和配置项
    /// @param root yaml节点对象
    static void loadFromYaml(YAML::Node& root);
    
    /// @brief 查找配置参数，返回其参数的基类指针
    /// @param name 配置名
    static ConfigVarBase::ptr lookupBase(const std::string& name);

    /// @brief 遍历配置模块所有配置项
    /// @param cb 每个配置项的回调函数
    static void visit(std::function<void(ConfigVarBase::ptr)> cb);

private:
    /// @brief 静态函数中静态变量，保证读写配置时锁一定初始化完毕
    /// @return 读写锁
    static RWMutexType& GetMutex(){
        static RWMutexType s_mutex;
        return s_mutex;
    }

    ///@brief 得到所有配置项
    static ConfigVarMap &GetDatas()
    {
        static ConfigVarMap s_datas;
        return s_datas;
    }
};


}