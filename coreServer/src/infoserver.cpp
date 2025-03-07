#include "infoserver.hpp"
#include "infoservice.hpp"
#include "json.hpp"
#include "logger.hpp"

#include <string>
#include <iostream>

using json = nlohmann::json;

InfoServer::InfoServer()
{
}

// 启动服务器
void InfoServer::start()
{

    Logger::output("InfoServer启动成功 监听端口: 8080", Logger::INFO);
    svr.listen("0.0.0.0", 8080);
}

// 设置 CORS 响应头
void InfoServer::setCorsHeaders(httplib::Response &res)
{
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");

    // 使用 set_header 设置 Content-Type
    res.set_header("Content-Type", "application/json");
}

void InfoServer::initMethods()
{
    // 设置OPTIONS方法的路由
    svr.Options(".*", [this](const httplib::Request &req, httplib::Response &res)
                {
                    setCorsHeaders(res);
                    res.status = 204; // No Content
                });

    // 设置GET方法的路由
    svr.Get("/getpackages", [this](const httplib::Request &req, httplib::Response &res)
            {
                setCorsHeaders(res);

                // 反序列化数据 通过消息id选择对应的处理函数进行处理
                auto msgHandler = InfoService::getInstance()->getHandler("getpackages");
                json js;
                msgHandler(res, js); });

    // 设置POST方法的路由
    svr.Post("/login", [this](const httplib::Request &req, httplib::Response &res)
             {
                 setCorsHeaders(res);

                 auto msgHandler = InfoService::getInstance()->getHandler("login");

                 json js = json::parse(req.body);
                 msgHandler(res, js); });

    svr.Post("/getrobotlist", [this](const httplib::Request &req, httplib::Response &res)
             {
                 setCorsHeaders(res);

                 auto msgHandler = InfoService::getInstance()->getHandler("getrobotlist");

                 json js = json::parse(req.body);
                 msgHandler(res, js); });

    svr.Post("/getrobotinfo", [this](const httplib::Request &req, httplib::Response &res)
             {
                 setCorsHeaders(res);

                 auto msgHandler = InfoService::getInstance()->getHandler("getrobotinfo");

                 json js = json::parse(req.body);
                 msgHandler(res, js); });

    svr.Post("/getonlinestatus", [this](const httplib::Request &req, httplib::Response &res)
             {
                 setCorsHeaders(res);

                 auto msgHandler = InfoService::getInstance()->getHandler("getonlinestatus");

                 json js = json::parse(req.body);
                 msgHandler(res, js); });

    svr.Post("/getallonlinestatus", [this](const httplib::Request &req, httplib::Response &res)
             {
                 setCorsHeaders(res);

                 auto msgHandler = InfoService::getInstance()->getHandler("getallonlinestatus");

                 json js = json::parse(req.body);
                 msgHandler(res, js); });

    svr.Post("/sendpackage", [this](const httplib::Request &req, httplib::Response &res)
             {
                 setCorsHeaders(res);

                 auto msgHandler = InfoService::getInstance()->getHandler("sendpackage");

                 json js = json::parse(req.body);
                 msgHandler(res, js); });

    svr.Post("/bulksendpackage", [this](const httplib::Request &req, httplib::Response &res)
             {
                 setCorsHeaders(res);

                 auto msgHandler = InfoService::getInstance()->getHandler("bulksendpackage");

                 json js = json::parse(req.body);
                 msgHandler(res, js); });

    svr.Post("/getdownloadedpackages", [this](const httplib::Request &req, httplib::Response &res)
             {
                 setCorsHeaders(res);

                 auto msgHandler = InfoService::getInstance()->getHandler("getdownloadedpackages");

                 json js = json::parse(req.body);
                 msgHandler(res, js); });

    svr.Post("/deploypackage", [this](const httplib::Request &req, httplib::Response &res)
             {
                 setCorsHeaders(res);

                 auto msgHandler = InfoService::getInstance()->getHandler("deploypackage");

                 json js = json::parse(req.body);
                 msgHandler(res, js); });

    svr.Post("/getdeployedpackagesinfo", [this](const httplib::Request &req, httplib::Response &res)
             {
                 setCorsHeaders(res);

                 auto msgHandler = InfoService::getInstance()->getHandler("getdeployedpackagesinfo");

                 json js = json::parse(req.body);
                 msgHandler(res, js); });
}
