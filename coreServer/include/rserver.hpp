#ifndef RSERVER_HPP
#define RSERVER_HPP

#include <websocketpp/config/asio.hpp>
#include <websocketpp/server.hpp>
#include <unordered_map>
#include <list>
#include "json.hpp"

using json = nlohmann::json;

using namespace std;

// 注意该服务器是单线程
// 表示一种WebSocket服务器类型(模板参数表明该服务器基于ASIO库实现了异步I/O)
typedef websocketpp::server<websocketpp::config::asio> server;
// connection_hdl表示WebSocket连接的句柄 可以用来标识和管理特定的连接。
using websocketpp::connection_hdl;

class RServer
{
public:
    static RServer *getInstance();

    // 启动服务
    void start(const std::string &ip, uint16_t port);

    // 新连接打开时触发的回调函数
    void on_open(connection_hdl hdl);
    // 连接关闭时触发的回调函数
    void on_close(connection_hdl hdl);
    // 接收到消息时触发的回调函数
    void on_message(connection_hdl hdl, server::message_ptr msg);

    // 获取指定机器人在线状态
    bool isOnline(uint32_t robotid);
    // 获取指定机器人的连接
    server::connection_ptr getConnection(uint32_t robotid);
    // 获取指定连接的所有最新消息
    json getMsg(server::connection_ptr conn, const string &type);

private:
    server m_server;
    unordered_map<uint32_t, server::connection_ptr> connectionMap;
    unordered_map<server::connection_ptr, list<json>> msgMap;

    // 定义互斥锁 保证connectionMap和msgMap的线程安全
    mutex _connMutex;
    // 定义条件变量 用于通知其他线程msgMap的更新
    condition_variable _cv;

    RServer();
    RServer(const RServer &other) = delete;
    RServer &operator=(const RServer &other) = delete;
};

#endif // RSERVER_HPP