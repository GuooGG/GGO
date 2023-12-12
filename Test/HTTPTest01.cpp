#include "GGo.h"

#define LOG GGO_LOG_DEBUG(GGO_LOG_ROOT())

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

int main(){
    // test_method_to_string();
    // test_string_to_method();
    // test_status_to_string();
    // test_request();
    test_response();
    return 0;
}