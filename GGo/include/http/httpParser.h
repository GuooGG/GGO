/**
 * @file httpParser.h
 * @author GGo
 * @brief HTTP请求与响应的解析封装类
 * @date 2023-12-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once
#include "http.h"
#include "../../../mongrel/http11_parser.h"
#include "../../../mongrel/httpclient_parser.h"


namespace GGo{
namespace HTTP{

/// @brief HTTP请求解析类
class HTTPRequestParser{
public:
    using ptr = std::shared_ptr<HTTPRequestParser>;

    /// @brief 无参构造函数
    HTTPRequestParser();

    /// @brief 解析HTTP请求协议
    /// @param data 文本数据
    /// @param len 文本数据长度
    /// @return 实际解析的长度，并将已经解析的数据移除
    size_t excute(char* data, size_t len);

    /// @brief 返回是否解析完成
    int isFinished();

    /// @brief 返回是否有错误
    int hasError();

    /// @brief 获取解析后的HTTP请求结构体
    HTTPRequest::ptr getData() const { return m_request; }

    /// @brief 设置错误
    /// @param error 错误码
    void setError(int error) { m_error = error; }

    /// @brief 获取消息体长度
    uint64_t getContentLength();

    /// @brief 获取解析结构体
    const http_parser& getParser() const { return m_parser; }
public:

    /// @brief 获取HTTP请求解析的缓存大小 
    static uint64_t getHTTPRequestBufferSize();

    /// @brief 获取HTTP请求解析的最大消息体大小
    static uint64_t getHTTPRequestMaxBodySize();

private:
    // 解析结构体
    http_parser m_parser;
    // 请求结构体
    HTTPRequest::ptr m_request;
    //错误码
    // 1000 : invalid method
    // 1001 : invalid version
    // 1002 : invalid field
    int m_error;
};

/// @brief HTTP响应解析类
class HTTPResponseParser{
public:
    using ptr = std::shared_ptr<HTTPResponseParser>;

    /// @brief 无参构造函数
    HTTPResponseParser();

    /// @brief 解析HTTP响应协议
    /// @param data 文本数据
    /// @param len 文本数据长度
    /// @param chunck 是否在解析chunck
    /// @return 实际解析的长度，并且移除已解析的数据
    size_t excute(char* data, size_t len, bool chunck);

    /// @brief 是否解析完成
    int isFinished();

    /// @brief 是否有错误
    int hasError();

    /// @brief 获取解析后的HTTP响应体
    HTTPResponse::ptr getData() const { return m_response; }

    /// @brief 设置错误
    /// @param error 错误码
    void setError(int error) { m_error = error; }

    /// @brief 获取消息体长度
    uint64_t getContentLength();

    /// @brief 获取解析结构体
    const httpclient_parser& getParser() const { return m_parser;}

public:

    /// @brief 获取HTTP响应解析缓存大小
    static uint64_t getHTTPResponseBufferSize();

    /// @brief 获取HTTP响应解析最大消息体大小
    static uint64_t getHTTPResponseMaxBodySize();

private:
    // 解析结构体
    httpclient_parser m_parser;
    // 响应结构体
    HTTPResponse::ptr m_response;
    //错误码
    // 1001 : invalid version
    // 1002 : invalid field
    int m_error;

};

}
}