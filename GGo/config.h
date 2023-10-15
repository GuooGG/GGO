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
#include<boost/lexical_cast.hpp>
#include"GGo/LogSystem.h"



namespace GGo{
class ConfigVarBase{
public:
    using ptr = std::shared_ptr<ConfigVarBase>;

    ConfigVarBase(const std::string& name,const std::string& description = "")
        :m_name(name)
        ,m_description(description)
    {}

    virtual ~ConfigVarBase() = default;

    virtual std::string toString() = 0;
    virtual bool fromString(const std::string& val) = 0;

    const std::string& getName() const{return m_name;}
    const std::string& getDescription() const {return m_description;}


protected:
    std::string m_name;
    std::string m_description;
};

template<class T>
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
            return boost::lexical_cast<std::string>(m_val);
        }catch (std::exception& e){
            //TODO::打印ERROR日志
        }
        return "";
    }
    bool fromString(const std::string& val) override{
        try{
            m_val = boost::lexical_cast<T>(val);
        }catch(std::exception& e){
            //TODO::打印ERROR日志
        }
        return false;
    }
    
privtae:
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
        const std::string& description = ""){
        auto temp = Lookup(name);
        if(tmp){
            //TODO::打印INFO日志
            return tmp;
        }
        //不能用小写字母，数字和特殊符号开头
        if(name.find_first_not_of("abcdefghikjlmnopqrstuvwxyz._012345678")
                != std::string::npos) {
            //TODO::打印ERROR日志
            throw std::invalid_argument(name);
        }
        typename ConfigVar<T>::ptr v(new ConfigVar<T>(name,default_value,description));
        m_data[name] = v;
        return v;

        

    }

    /// @brief 查找配置参数
    /// @param name 配置参数名称
    /// @return 返回配置参数名为name的配置参数 
    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name){
        auto it = m_data.find(name);
        if(it == m_data.end()){
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
    }



private:
    static ConfigVarMap m_data;

};


}