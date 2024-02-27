/**
 * @file servlet.h
 * @author GGO
 * @brief httpServlet模块封装
 * @version 0.1
 * @date 2024-02-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include<memory>
#include<functional>
#include<string>
#include<vector>
#include<unordered_map>
#include"http/http.h"
#include"http/httpSession.h"
#include"util.h"
#include"thread.h"

namespace GGo{
namespace HTTP{

class servlet
{
public:
    using ptr = std::shared_ptr<servlet>;

    /// @brief 构造函数
    /// @param name 名称
    servlet(const std::string& name)
        :m_name(name){}

    /// @brief 析构函数
    virtual ~servlet(){}

    /// @brief 处理请求
    /// @param request HTTP请求
    /// @param response HTTP响应
    /// @param session HTTP连接
    /// @return 是否处理成功
    virtual int32_t handle(GGo::HTTP::HTTPRequest::ptr request
                        , GGo::HTTP::HTTPResponse::ptr response
                        , GGo::HTTP::HTTPSession::ptr session) = 0;
    
    /// @brief 返回servlet名称 
    const std::string& getName() const { return m_name; }    

protected:
    std::string m_name;

};

/// @brief 函数式servlet
class functionServlet : public servlet{
public:
    using ptr = std::shared_ptr<functionServlet>;
    using callback = std::function<int32_t(GGo::HTTP::HTTPRequest::ptr
                        , GGo::HTTP::HTTPResponse::ptr
                        , GGo::HTTP::HTTPSession::ptr)>;

    /// @brief 构造函数
    /// @param cb 回调函数
    functionServlet(callback cb);

    virtual int32_t handle(GGo::HTTP::HTTPRequest::ptr request
                        , GGo::HTTP::HTTPResponse::ptr response
                        , GGo::HTTP::HTTPSession::ptr session) override;

private:    
    callback m_callback;
};



class IServeltCreator{
public:
    using ptr = std::shared_ptr<IServeltCreator>;
    virtual ~IServeltCreator(){}
    virtual std::string getName() const = 0;
    virtual servlet::ptr get() const = 0;

};

class HoldServletCreator : public IServeltCreator{
public:
    using ptr = std::shared_ptr<HoldServletCreator>;

    HoldServletCreator(servlet::ptr slt)
        :m_servlet(slt){}
    
    servlet::ptr get() const override{
        return m_servlet;
    }

    std::string getName() const override{
        return m_servlet->getName();
    }

private:
    servlet::ptr m_servlet;
};


template<class T>
class ServletCreator : public IServeltCreator{
public:
    using ptr = std::shared_ptr<ServletCreator>;

    ServletCreator(){}

    servlet::ptr get() const override{
        return servlet::ptr(new T);
    }

    std::string getName() const override{
        return typeToName<T>();
    }
};

/// @brief Servlet分发器
class ServletDispatch : public servlet{
public:
    using ptr = std::shared_ptr<ServletDispatch>;
    using RWMutexType = RWMutex;

    /// @brief 构造函数
    ServletDispatch();

    virtual int32_t handle(GGo::HTTP::HTTPRequest::ptr request
                        , GGo::HTTP::HTTPResponse::ptr response
                        , GGo::HTTP::HTTPSession::ptr session) override;

    /// @brief 添加servlet
    /// @param uri uri
    /// @param slt servlet
    void addServlet(const std::string& uri, servlet::ptr slt);

    /// @brief 添加servlet
    /// @param uri uri
    /// @param cb functionservlet回调函数
    void addServlet(const std::string& uri, functionServlet::callback cb);

    /// @brief 添加模糊匹配servlet
    /// @param uri uri
    /// @param slt servlet
    void addGlobServlet(const std::string& uri, servlet::ptr slt);

    /// @brief 添加模糊匹配servlet
    /// @param uri uri
    /// @param slt functionservlet回调函数
    void addGlobServlet(const std::string& uri, functionServlet::callback cb);

    void addServletCreator(const std::string& uri, IServeltCreator::ptr creator);
    void addGlobServletCreator(const std::string& uri, IServeltCreator::ptr creator);

    template<class T>
    void addServletCreator(const std::string& uri){
        addServletCreator(uri, std::make_shared<ServletCreator<T> >());
    }

    template<class T>
    void addGlobServletCreator(const std::string& uri){
        addServletCreator(uri, std::make_shared<ServletCreator<T> >());
    }

    /// @brief 删除servlet
    /// @param uri uri
    void delServlet(const std::string& uri);

    /// @brief 删除模糊匹配servlet
    /// @param uri uri
    void delGlobServlet(const std::string& uri);

    /// @brief 返回默认servlet
    servlet::ptr getDefault() const {return m_default; }

    /// @brief 设置默认servlet 
    void setDefault(servlet::ptr v) { m_default = v; }

    /// @brief 获得精准匹配的servlet
    /// @param uri uri
    /// @return 对应的servlet
    servlet::ptr getServlet(const std::string& uri);

    /// @brief 获得模糊匹配的servlet
    /// @param uri uri
    /// @return 对应的servlet
    servlet::ptr getGlobServlet(const std::string& uri);

    /// @brief 通过uri获取对应的servlet,优先精准匹配，其次模糊匹配
    /// @param uri uri
    /// @return 对应的servlet
    servlet::ptr getMatchedServlet(const std::string& uri);

    void listAllServletCreator(std::map<std::string, IServeltCreator::ptr>& infos);
    void listAllGlobServletCerator(std::map<std::string, IServeltCreator::ptr>& infos);

private:
    // 读写锁
    RWMutexType m_mutex;
    // servlet map 精准匹配
    std::unordered_map<std::string, IServeltCreator::ptr> m_datas;
    // servlet map 模糊匹配
    std::vector<std::pair<std::string, IServeltCreator::ptr> > m_globs;
    // 默认servlet
    servlet::ptr m_default;

};



class NotFoundServlet : public servlet{
public:
    using ptr = std::shared_ptr<NotFoundServlet>;

    /// @brief 构造函数
    NotFoundServlet(const std::string& name);

    virtual int32_t handle(GGo::HTTP::HTTPRequest::ptr request
                        , GGo::HTTP::HTTPResponse::ptr response
                        , GGo::HTTP::HTTPSession::ptr session) override;

private:
    std::string m_name;
    std::string m_content;
};

}
}