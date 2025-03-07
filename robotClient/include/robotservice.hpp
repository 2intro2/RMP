#ifndef ROBOTSERVICE_HPP
#define ROBOTSERVICE_HPP

#include <unordered_map>
#include <functional>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio.hpp>
#include <iostream>
#include <string>

#include "json.hpp"

// 表示一种WebSocket服务器类型(模板参数表明该服务器基于ASIO库实现了异步I/O)
typedef websocketpp::client<websocketpp::config::asio> client;

using json = nlohmann::json;
using websocketpp::connection_hdl;
using namespace std;
// 表示回调函数的类型
using MsgHandler = function<void(client &, connection_hdl &, json &)>;

class RobotService
{
public:
    // 全局访问节点
    static RobotService *getInstance();
    // 获取消息处理器
    MsgHandler getMsgHandler(string type);

    // 获取机器人的IP地址
    void getIP(client &m_client, connection_hdl &hdl, json &js);
    // 获取机器人已下载的软件包列表
    void getDownloadedPackages(client &m_client, connection_hdl &hdl, json &js);
    // 部署指定软件包
    void deployPackage(client &m_client, connection_hdl &hdl, json &js);
    // 获取已部署软件包的信息
    void getDeployedPackagesInfo(client &m_client, connection_hdl &hdl, json &js);

private:
    // 使用哈希表存储消息id与其对应的处理方法
    std::unordered_map<string, MsgHandler> _msgHandlerMap;
    boost::asio::io_context io_context_;
    // 单例模式
    RobotService();
    RobotService &operator=(const RobotService &rhs) = delete;
    RobotService(const RobotService &) = delete;
};

#endif