#include "httpParser.h"
#include "logSystem.h"
#include "config.h"

namespace GGo{
namespace HTTP{

static GGo::Logger::ptr g_logger = GGO_LOG_NAME("system");

static GGo::ConfigVar<uint64_t>::ptr g_http_request_buffer_size = 
            GGo::Config::Lookup("http.request.buffer_size", 
                        (uint64_t)(4 * 1024), "http request buffer size");

static GGo::ConfigVar<uint64_t>::ptr g_http_request_max_body_size = 
            GGo::Config::Lookup("http.request.max_body_size",
                        (uint64_t)(64 * 1024 * 1024), "http request max body size");

static GGo::ConfigVar<uint64_t>::ptr g_http_response_buffer_size = 
            GGo::Config::Lookup("http.response.buffer_size",
                        (uint64_t)(4 * 1024), "http response buffer size");

static GGo::ConfigVar<uint64_t>::ptr g_http_response_max_body_size = 
            GGo::Config::Lookup("http.response.max_body_size",
                        (uint64_t)(64 * 1024 * 1024), "http response max body size");

static uint64_t s_http_request_buffer_size = 0;
static uint64_t s_http_request_max_body_size = 0;
static uint64_t s_http_response_buffer_size = 0;
static uint64_t s_http_response_max_body_size = 0;

void on_request_method(void* data, const char* at, size_t len){
    HTTPRequestParser* parser = static_cast<HTTPRequestParser*>(data);
    HTTPMethod method = StringToHTTPMethod(at);

    if(method == HTTPMethod::INVALID_METHOD){
        GGO_LOG_WARN(g_logger) << "invalid http request method: " 
                            << std::string(at, len);
        parser->setError(1000);
        return;
    }
    parser->getData()->setMethod(method);
}

void on_request_uri(void* data, const char* at, size_t len){

}

void on_request_fragment(void* data, const char* at, size_t len){
    HTTPRequestParser* parser = static_cast<HTTPRequestParser*>(data);
    parser->getData()->setFrgment(std::string(at, len));
}

void on_request_path(void* data, const char* at, size_t len){
    HTTPRequestParser* parser = static_cast<HTTPRequestParser*>(data);
    parser->getData()->setPath(std::string(at, len));
}

void on_request_query(void *data, const char *at, size_t length) {
    HTTPRequestParser* parser = static_cast<HTTPRequestParser*>(data);
    parser->getData()->setQuery(std::string(at, length));
}

void on_request_version(void* data, const char* at, size_t len){
    HTTPRequestParser* parser = static_cast<HTTPRequestParser*>(data);
    uint8_t version = 0x00;
    if(strncmp(at, "HTTP/1.0", len) == 0){
        version = 0x10;
    }else if(strncmp(at, "HTTP/1.1", len) == 0){
        version = 0x11;
    }else{
        GGO_LOG_WARN(g_logger) << "invalid http version: "
                            << std::string(at, len);
        parser->setError(1001);
        return;
    }
    parser->getData()->setVersion(version);
}

void on_request_header_done(void *data, const char *at, size_t length) {

}

void on_request_http_field(void* data, const char* field, size_t flen, const char* value, size_t vlen){
    HTTPRequestParser* parser = static_cast<HTTPRequestParser*>(data);
    if(flen == 0){
        GGO_LOG_WARN(g_logger) << "invalid http request filed length == 0";
        parser->setError(1002);
        return;
    }
    parser->getData()->setHeader(std::string(field, flen), std::string(value, vlen));
}


HTTPRequestParser::HTTPRequestParser()
    :m_error(0)
{
    m_request.reset(new GGo::HTTP::HTTPRequest);
    http_parser_init(&m_parser);

    m_parser.request_method = on_request_method;
    m_parser.request_uri = on_request_uri;
    m_parser.fragment = on_request_fragment;
    m_parser.request_path = on_request_path;
    m_parser.query_string = on_request_query;
    m_parser.http_version = on_request_version;
    m_parser.header_done = on_request_header_done;
    m_parser.http_field = on_request_http_field;
    m_parser.data = this;
}

uint64_t HTTPRequestParser::getContentLength()
{
    return m_request->getHeaderAs<uint64_t>("content-length", 0);
}

uint64_t HTTPRequestParser::getHTTPRequestBufferSize()
{
    return s_http_request_buffer_size;
}

uint64_t HTTPRequestParser::getHTTPRequestMaxBodySize()
{
    return s_http_request_max_body_size;
}
uint64_t HTTPResponseParser::getHTTPResponseBufferSize()
{
    return s_http_response_buffer_size;
}
uint64_t HTTPResponseParser::getHTTPResponseMaxBodySize()
{
    return s_http_request_max_body_size;
}

namespace{
struct _RequestSizeIniter{
    _RequestSizeIniter(){
        s_http_request_buffer_size = g_http_request_buffer_size->getValue();
        s_http_request_max_body_size = g_http_request_max_body_size->getValue();
        s_http_response_buffer_size = g_http_response_buffer_size->getValue();
        s_http_response_max_body_size = g_http_response_max_body_size->getValue();

        g_http_request_buffer_size->addListener(
            [](const uint64_t& oldv, const uint64_t& newv){
                s_http_request_buffer_size = newv;
            }
        );

        g_http_request_max_body_size->addListener(
            [](const uint64_t& oldv, const uint64_t& newv){
                s_http_request_max_body_size = newv;
            }
        );
        
        g_http_response_buffer_size->addListener(
            [](const uint64_t& oldv, const uint64_t& newv){
                s_http_response_buffer_size = newv;
            }
        );

        g_http_response_max_body_size->addListener(
            [](const uint64_t& oldv, const uint64_t& newv){
                s_http_response_max_body_size = newv;
            }
        );

    }
};

static _RequestSizeIniter _request_init;
}



}
}