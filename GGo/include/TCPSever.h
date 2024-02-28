/**
 * @file TCPSever.h
 * @author GGo
 * @brief TCP服务器封装
 * @date 2023-12-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include <memory>
#include <functional>
#include "address.h"
#include "ioScheduler.h"
#include "socket.h"
#include "nonCopyable.h"
#include "config.h"

namespace GGo{

/// @brief TCP服务器定义结构体
struct TCPSeverConf{
    using ptr = std::shared_ptr<TCPSeverConf>;

    std::vector<std::string> addresses;
    int keepalive = 0;
    int timeout = 1000 * 2 * 60;
    int ssl = 0;
    std::string id;

    std::string type = "http";
    std::string name;
    std::string cert_file;
    std::string key_file;
    std::string accept_worker;
    std::string io_worker;
    std::string process_worker;
    std::map<std::string, std::string> args;

    bool isValid() const {
        return !addresses.empty();
    }

    bool operator==(const TCPSeverConf& oth) const {
        return addresses == oth.addresses
            && keepalive == oth.keepalive
            && timeout == oth.timeout
            && name == oth.name
            && ssl == oth.ssl
            && cert_file == oth.cert_file
            && key_file == oth.key_file
            && accept_worker == oth.accept_worker
            && io_worker == oth.io_worker
            && process_worker == oth.process_worker
            && args == oth.args
            && id == oth.id
            && type == oth.type;
    }
};

template<>
class LexicalCast<std::string, TCPSeverConf>{
public:
    TCPSeverConf operator()(const std::string& str){
        YAML::Node node = YAML::Load(str);
        TCPSeverConf conf;
        conf.id = node["id"].as<std::string>(conf.id);
        conf.type = node["type"].as<std::string>(conf.type);
        conf.keepalive = node["keepalive"].as<int>(conf.keepalive);
        conf.timeout = node["timeout"].as<int>(conf.timeout);
        conf.name = node["name"].as<std::string>(conf.name);
        conf.ssl = node["ssl"].as<int>(conf.ssl);
        conf.cert_file = node["cert_file"].as<std::string>(conf.cert_file);
        conf.key_file = node["key_file"].as<std::string>(conf.key_file);
        conf.accept_worker = node["accept_worker"].as<std::string>();
        conf.io_worker = node["io_worker"].as<std::string>();
        conf.process_worker = node["precess_worker"].as<std::string>();
        conf.args = LexicalCast<std::string,
                    std::map<std::string, std::string>>()(node["args"].as<std::string>(""));

        if(node["addresses"].IsDefined()){
            for(size_t i = 0; i < node["addresses"].size(); i++){
                conf.addresses.push_back(node["addresses"][i].as<std::string>());
            }
        }
        return conf;
    }
};

template<>
class LexicalCast<TCPSeverConf, std::string>{
public:
    std::string operator()(const TCPSeverConf& conf){
        YAML::Node node;
        node["id"] = conf.id;
        node["type"] = conf.type;
        node["name"] = conf.name;
        node["keepalive"] = conf.keepalive;
        node["timeout"] = conf.timeout;
        node["ssl"] = conf.ssl;
        node["cert_file"] = conf.cert_file;
        node["key_file"] = conf.key_file;
        node["accept_worker"] = conf.accept_worker;
        node["io_worker"] = conf.io_worker;
        node["precess_worker"] = conf.process_worker;
        node["args"] = YAML::Load(LexicalCast<std::map<std::string, std::string>
            , std::string>()(conf.args));
        for(auto& addr : conf.addresses){
            node["addresses"].push_back(addr);
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};



class TCPSever : public std::enable_shared_from_this<TCPSever>
                    , nonCopyable
{
public:
    using ptr = std::shared_ptr<TCPSever>;

    TCPSever(GGo::IOScheduler* worker = GGo::IOScheduler::getThis(),
            GGo::IOScheduler* io_worker = GGo::IOScheduler::getThis(),
            GGo::IOScheduler* accept_worker = GGo::IOScheduler::getThis());

    
    virtual ~TCPSever();


    /// @brief 绑定地址
    /// @param addr 待绑定的地址
    /// @param ssl 是否使用SSlSocket
    /// @return 是否绑定成功
    virtual bool bind(GGo::Address::ptr addr, bool ssl = false);


    /// @brief 批量绑定地址
    /// @param addrs 待绑定的地址数组
    /// @param fails 绑定失败的地址数组
    /// @param ssl 是否使用SSLScocket
    /// @return 是否绑定成功
    virtual bool bind(const std::vector<Address::ptr>& addrs
                    , std::vector<Address::ptr>& fails,
                    bool ssl = false);

    /// @brief 启动服务
    /// @pre 需要bind成功
    virtual bool start();

    virtual void stop();

    /// @brief 获取读取超时时间 
    uint64_t getRecvTimeout() const { return m_recvTimeout; }

    /// @brief 获取服务器名称 
    std::string getName() const { return m_name; }

    /// @brief 设置读取超时时间
    void setRecvTimeout(uint64_t timeout) { m_recvTimeout = timeout; }

    /// @brief 设置服务器名称 
    virtual void setName(const std::string& name) { m_name = name; }

    /// @brief 返回服务是否停止 
    bool isStop() const { return m_isStop; }

    /// @brief 获取被监听的socks数组 
    std::vector<Socket::ptr> getSocks() const { return m_socks; }

    /// @brief 获取TCPSever配置项结构体 
    TCPSeverConf::ptr getConf() const { return m_conf;}

    /// @brief 设置TCPSever配置
    /// @param conf 新配置项结构体
    void setConf(TCPSeverConf::ptr conf) { m_conf = conf; }
    void setConf(const TCPSeverConf& conf);

    /// @brief 将TCPSever对象转换为字符串输出
    /// @param prefix 输出前缀，默认为空
    virtual std::string toString(const std::string& prefix = "");

protected:
    
    /// @brief 处理客户端连接
    /// @param cilent 客户端socket
    virtual void handleCilent(Socket::ptr cilent);

    /// @brief 开始接受连接
    /// @param sock 
    virtual void startAccept(Socket::ptr sock);

protected:
    // 被监听的sock数组
    std::vector<Socket::ptr> m_socks;
    IOScheduler* m_worker;
    IOScheduler* m_ioWorker;
    IOScheduler* m_acceptWorker;
    
    // 接收超时时间(ms)
    uint64_t m_recvTimeout;
    // 服务器名称
    std::string m_name;
    // 服务器类型
    std::string m_type = "tcp";
    // 服务器是否停止
    bool m_isStop;
    bool m_ssl;

    // TCPSever配置结构体
    TCPSeverConf::ptr m_conf;
};



}