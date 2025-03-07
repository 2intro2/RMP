#ifndef INFOSERVER_HPP
#define INFOSERVER_HPP

#include "httplib.h"

class InfoServer
{
public:
    InfoServer();
    // 启动服务器
    void start();
    // 设置GET方法和POST方法的路由
    void initMethods();
    // 设置 CORS 响应头
    void setCorsHeaders(httplib::Response &res);

private:
    httplib::Server svr;
};

#endif