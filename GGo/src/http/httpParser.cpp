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

size_t HTTPRequestParser::excute(char *data, size_t len)
{
    size_t offset = http_parser_execute(&m_parser, data, len, 0);
    memmove(data, data + offset, len - offset);
    return offset;
}

int HTTPRequestParser::isFinished()
{
    return http_parser_is_finished(&m_parser);
}

int HTTPRequestParser::hasError()
{
    return m_error || http_parser_has_error(&m_parser);
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

uint64_t HTTPResponseParser::getContentLength()
{
    return m_response->getHeaderAs<uint64_t>("content_length", 0);
}

uint64_t HTTPResponseParser::getHTTPResponseBufferSize()
{
    return s_http_response_buffer_size;
}
uint64_t HTTPResponseParser::getHTTPResponseMaxBodySize()
{
    return s_http_request_max_body_size;
}

void on_response_reason(void* data, const char* at, size_t len){
    HTTPResponseParser* parser = static_cast<HTTPResponseParser*>(data);
    parser->getData()->setReasons(std::string(at,len));
}

void on_response_status(void* data, const char* at, size_t len){
    HTTPResponseParser* parser = static_cast<HTTPResponseParser*>(data);
    parser->getData()->setStatus((HTTPStatus)(atoi(at)));
}

void on_response_chunksize(void* data, const char* at, size_t len){
    
}

void on_response_version(void* data, const char* at, size_t len){
    HTTPResponseParser* parser = static_cast<HTTPResponseParser*>(data);
    uint8_t version = 0x00;
    if(strncmp(at, "HTTP/1.1", len) == 0){
        version = 0x11;
    }else if(strncmp(at, "HTTP/1.0", len) == 0){
        version = 0x10;
    }else{
        GGO_LOG_WARN(g_logger) << "invalid http response version: "
                            << std::string(at, len);
        parser->setError(1001);
        return;
    }
    parser->getData()->setVersion(version);
}

void on_response_header_done(void *data, const char *at, size_t length) {

}

void on_response_last_chunk(void *data, const char *at, size_t length) {

}

void on_response_http_field(void *data, const char *field, size_t flen
                           ,const char *value, size_t vlen) {
    HTTPResponseParser* parser = static_cast<HTTPResponseParser*>(data);
    if(flen == 0) {
        GGO_LOG_WARN(g_logger) << "invalid http response field length == 0";
        parser->setError(1002);
        return;
    }
    parser->getData()->setHeader(std::string(field, flen)
                                ,std::string(value, vlen));
}

HTTPResponseParser::HTTPResponseParser()
    :m_error(0)
{
    m_response.reset(new GGo::HTTP::HTTPResponse);
    httpclient_parser_init(&m_parser);

    m_parser.reason_phrase = on_response_reason;
    m_parser.status_code = on_response_status;
    m_parser.chunk_size = on_response_chunksize;
    m_parser.http_version = on_response_version;
    m_parser.header_done = on_response_header_done;
    m_parser.last_chunk = on_response_last_chunk;
    m_parser.http_field = on_response_http_field;
    m_parser.data = this;

}

size_t HTTPResponseParser::excute(char *data, size_t len, bool chunck)
{
    if(chunck){
        httpclient_parser_init(&m_parser);
    }
    size_t offset = httpclient_parser_execute(&m_parser, data, len, 0);

    memmove(data, data + offset, len - offset);
    return offset;
}

int HTTPResponseParser::isFinished()
{
    return httpclient_parser_is_finished(&m_parser);
}

int HTTPResponseParser::hasError()
{
    return m_error || httpclient_parser_has_error(&m_parser);
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