#pragma once

#include<memory>
#include<sstream>
#include<string>
#include<exception>
//boost::lexical_cast操作符，将字符串转为数字类型
//std::stoi(),std::stod(),std::to_string()
//lexical<TargetType>(SrcType);
//TODO::两者相比有什么区别
#include<boost/lexical_cast.hpp>
#include"GGo/config.h"



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
            //打印ERROR日志
            catch(std::expection& e){

            }
        }
        return "";
    }
    
privtae:
    T m_val;
};



}