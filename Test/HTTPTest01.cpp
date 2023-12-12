#include "GGo.h"

#define LOG GGO_LOG_DEBUG(GGO_LOG_ROOT())

const char test_request_data[] = "POST / HTTP/1.1\r\n"
                                "Host: www.sylar.top\r\n"
                                "Content-Length: 10\r\n\r\n"
                                "1234567890";
const char test_response_data[] = "HTTP/1.1 200 OK\r\n"
        "Date: Tue, 04 Jun 2019 15:43:56 GMT\r\n"
        "Server: Apache\r\n"
        "Last-Modified: Tue, 12 Jan 2010 13:48:00 GMT\r\n"
        "ETag: \"51-47cf7e6ee8400\"\r\n"
        "Accept-Ranges: bytes\r\n"
        "Content-Length: 81\r\n"
        "Cache-Control: max-age=86400\r\n"
        "Expires: Wed, 05 Jun 2019 15:43:56 GMT\r\n"
        "Connection: Close\r\n"
        "Content-Type: text/html\r\n\r\n"
        "<html>\r\n"
        "<meta http-equiv=\"refresh\" content=\"0;url=http://www.baidu.com/\">\r\n"
        "</html>\r\n";


void test_method_to_string(){
    LOG << "test method to string start";
    GGo::HTTP::HTTPMethod method = GGo::HTTP::HTTPMethod::DELETE;
    LOG << GGo::HTTP::HTTPMethodToString(method);
}

void test_string_to_method(){
    LOG << "test string to method start";
    std::string str = "GET";
    GGo::HTTP::HTTPMethod method = GGo::HTTP::StringToHTTPMethod(str);
    LOG << GGo::HTTP::HTTPMethodToString(method);
}

void test_status_to_string(){
    LOG << "test status to string start";
    GGo::HTTP::HTTPStatus status = GGo::HTTP::HTTPStatus::ACCEPTED;
    LOG << GGo::HTTP::HTTPStatusToString(status);
}

void test_request(){
    GGo::HTTP::HTTPRequest::ptr req(new GGo::HTTP::HTTPRequest);
    req->setHeader("host", "www.inkgocloud.cn");
    req->setBody("hello ggo");
    req->dump(std::cout) << std::endl;
}

void test_response(){
    GGo::HTTP::HTTPResponse::ptr rsp(new GGo::HTTP::HTTPResponse);
    rsp->setHeader("X-X", "ggo");
    rsp->setBody("hello ggo");
    rsp->setStatus((GGo::HTTP::HTTPStatus)400);
    rsp->setAutoClose(false);
    rsp->setCookie("12","34");

    rsp->dump(std::cout) << std::endl;
}

void test_request_parser(){
    GGo::HTTP::HTTPRequestParser parser;
    std::string tmp = test_request_data;
    size_t s = parser.excute(&tmp[0], tmp.size());
    LOG << "execute rt=" << s
        << "has_error=" << parser.hasError()
        << " is_finished=" << parser.isFinished()
        << " total=" << tmp.size()
        << " content_length=" << parser.getContentLength();
    tmp.resize(tmp.size() - s);
    LOG << parser.getData()->toString();
    LOG << "============================";
    LOG << tmp;
}


void test_response_parser(){
    GGo::HTTP::HTTPResponseParser parser;
    std::string tmp = test_response_data;
    size_t s = parser.excute(&tmp[0], tmp.size(), true);
    LOG << "execute rt=" << s
        << " has_error=" << parser.hasError()
        << " is_finished=" << parser.isFinished()
        << " total=" << tmp.size()
        << " content_length=" << parser.getContentLength()
        << " tmp[s]=" << tmp[s];
    tmp.resize(tmp.size() - s);
    LOG << parser.getData()->toString();
    LOG << "============================";
    LOG << tmp;
}

int main(){
    // test_method_to_string();
    // test_string_to_method();
    // test_status_to_string();
    // test_request();
    // test_response();
    // test_request_parser();
    test_response_parser();
    return 0;
}