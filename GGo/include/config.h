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
//boost::lexical_cast操作符，将字符串转为数字类型
//std::stoi(),std::stod(),std::to_string()
//lexical<TargetType>(SrcType);
//TODO::两者相比有什么区别
#include<yaml-cpp/yaml.h>
#include<boost/lexical_cast.hpp>
#include"LogSystem.h"



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

/// @brief 配置量类型
/// @tparam T 配置量的值类型
/// @tparam FromStr 用于从字符串转为值类型的类型
/// @tparam ToStr 用于从值类型转为字符串的类型
template<class T ,class FromStr = LexicalCast< std::string, T>
                 ,class ToStr = LexicalCast< T, std::string> >
class ConfigVar : public ConfigVarBase{

public:
    using ptr = std::shared_ptr<ConfigVar>;

    ConfigVar(const std::string& name,const T& default_value
            ,const std::string& description)
            :ConfigVarBase(name,description)
            ,m_val(default_value)
            {}
    std::string toString() override{
        try{
            return ToStr()(m_val);
        }catch (std::exception& e){
            //TODO::打印ERROR日志
        }
        return "";
    }
    bool fromString(const std::string& val) override{
        try{
            m_val = FromStr()(val);
        }catch(std::exception& e){
            GGO_LOG_ERROR(GGO_LOG_ROOT()) << "ConfigVar::fromstring exception " 
                                          << e.what() << " convert string to XXX"
                                          << " name=" << m_name
                                          << " - " << val;
        }
        return false;
    }

    const T getValue() const { return m_val;}
    
private:
    T m_val;
};

class Config{
public:
    using ptr = std::shared_ptr<Config>;
    using ConfigVarMap = std::unordered_map<std::string,ConfigVarBase::ptr>;


    //TODO：：C++模板编程中typename的作用，用以区分歧义
    /// @brief 获取/创建对应参数名的配置参数
    /// @param name 配置参数名称
    /// @param default_value 参数默认值
    /// @param description 参数描述
    /// @details 获取参数名为name的配置参数，如果存在直接返回
    ///          如果不存在，则创建该配置参数并用default_value赋值
    /// @return 返回对应的配置啊承诺书，如果参数名存在但是类型不匹配则返回nulptr
    /// @exception 如果参数名包含非法字符[^0-9a-z_.] 则抛出异常 std::valid_argument
    template<class T>
    static typename ConfigVar<T>::ptr Lookup(
        const std::string& name,
        const T& default_value,
        const std::string& description = "")
{
        auto it = GetDatas().find(name);
        if(it != GetDatas().end()){
            auto tmp = std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
            if(tmp){
                GGO_LOG_INFO(GGO_LOG_ROOT()) << "Lookup name=" << name << " exists";
                return tmp; 
            }else{
                GGO_LOG_ERROR(GGO_LOG_ROOT()) << "Lookup name=" << name << " exists but type not right";
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

    ///@brief 得到所有配置项 
    static ConfigVarMap& GetDatas(){
        static ConfigVarMap s_datas;
        return s_datas;
    }

private:

    
};


}