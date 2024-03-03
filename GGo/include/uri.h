/**
 * @file uri.h
 * @author GGO
 * @brief URI解析封装类
 * @version 0.1
 * @date 2024-03-03
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <memory>
#include <string>
#include <stdint.h>
#include "address.h"

namespace GGo{


/**
 * @brief 
 * 
 * 
 * 
 */

class Uri{
public:
    using ptr = std::shared_ptr<Uri>;

    /// @brief 创建Uri对象方法
    /// @param uri uri字符串
    /// @return 返回解析成功的uri对象
    static Uri::ptr Create(const std::string& uri);

    /// @brief 构造函数
    Uri();

    /// @brief 返回Scheme 
    const std::string& getScheme() const { return m_scheme; }

    /// @brief 返回userinfo 
    const std::string& getUserinfo() const { return m_userinfo; }

    /// @brief 返回host 
    const std::string& getHost() const { return m_host; }

    /// @brief 返回path 
    const std::string& getPath() const { return m_path; }

    /// @brief 返回query 
    const std::string& getQuery() const { return m_query; }

    /// @brief 返回fragment 
    const std::string& getFragment() const { return m_fragment; } 

    /// @brief 返回端口号 
    int32_t getPort() const;

    /// @brief 设置scheme
    /// @param value scheme
    void setScheme(const std::string& value){
        m_scheme = value;
    }

    /// @brief 设置userinfo
    /// @param value userinfo
    void setUserinfo(const std::string& value){
        m_userinfo = value;
    }

    /// @brief 设置host
    /// @param value host
    void setHost(const std::string& value){
        m_host = value;
    }

    /// @brief 设置path
    /// @param value path
    void setPath(const std::string& value){
        m_path = value;
    }

    /// @brief 设置query
    /// @param value query
    void setQuery(const std::string& value){
        m_query = value;
    }

    /// @brief 设置fragment
    /// @param value fragment
    void setFragment(const std::string& value){
        m_fragment = value;
    }

    /// @brief 设置端口号
    /// @param value 端口号
    void setPort(int32_t value){
        m_port = value;
    }

    /// @brief 序列化输出到流
    /// @param os 目标输出流
    /// @return 输出流
    std::ostream& dump(std::ostream& os) const;

    /// @brief 转为字符串 
    std::string toString() const;

    /// @brief 获取对应的Address 
    Address::ptr createAddress() const; 
private:
    /// @brief 是否是默认端口
    bool isDefaultPort() const;

private:
    
    std::string m_scheme;
    std::string m_userinfo;
    std::string m_host;
    std::string m_path;
    std::string m_query;
    std::string m_fragment;
    int32_t m_port;

};

}