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


    virtual bool bind(GGo::Address::ptr addr, bool ssl = false);


    virtual bool bind(const std::vector<Address::ptr>& addrs
                    , std::vector<Address::ptr>& fails,
                    bool ssl = false);

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

private:
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
};



}