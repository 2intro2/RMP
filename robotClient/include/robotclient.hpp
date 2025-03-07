#ifndef ROBOTCLIENT_HPP
#define ROBOTCLIENT_HPP

#include <iostream>
#include <websocketpp/config/asio.hpp>
#include <websocketpp/client.hpp>
#include <fstream>

// 表示一种WebSocket服务器类型(模板参数表明该服务器基于ASIO库实现了异步I/O)
typedef websocketpp::client<websocketpp::config::asio> client;
// connection_hdl表示WebSocket连接的句柄 可以用来标识和管理特定的连接。
using websocketpp::connection_hdl;

class RobotClient
{
public:
    RobotClient(uint32_t robotid = 0);

    // 启动服务
    void start(const std::string &ip, uint16_t port);

    // 重新连接
    void reconnect();
    // 判断文件是否存在
    bool isFileExists(const std::string &file_path);
    // 计算文件的MD5值
    std::string getMD5SUM(const std::string &file_path);

private:
    // 新连接打开时触发的回调函数
    void on_open(connection_hdl hdl);
    // 连接关闭时触发的回调函数
    void on_close(connection_hdl hdl);
    // 连接出错时触发的回调函数
    void on_fail(connection_hdl hdl);
    // 接收到消息时触发的回调函数
    void on_message(connection_hdl hdl, client::message_ptr msg);

    client m_client;
    boost::asio::io_context m_io_context;
    uint32_t m_robotid;

    std::string m_ip;
    uint16_t m_port;

    // 文件传输相关
    std::ofstream file;
    bool is_file_open;
    bool is_file_transfer;
    size_t expected_file_size;
    size_t received_file_size;
};

#endif