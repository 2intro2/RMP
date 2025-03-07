#include "rserver.hpp"
#include "logger.hpp"

RServer *RServer::getInstance()
{
    static RServer instance;
    return &instance;
}

RServer::RServer()
{
    // 注册回调函数
    m_server.set_open_handler(bind(&RServer::on_open, this, std::placeholders::_1));
    m_server.set_close_handler(bind(&RServer::on_close, this, std::placeholders::_1));
    m_server.set_message_handler(bind(&RServer::on_message, this, std::placeholders::_1, std::placeholders::_2));
}

void RServer::start(const std::string &ip, uint16_t port)
{
    // 设置日志级别
    m_server.set_access_channels(websocketpp::log::alevel::all);

    // 关闭帧头日志
    m_server.clear_access_channels(websocketpp::log::alevel::frame_header);

    // 关闭帧负载日志
    m_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

    // 初始化ASIO库  设置异步I/O
    m_server.init_asio();
    // 允许重用地址 帮助快速重启服务器
    m_server.set_reuse_addr(true);

    m_server.set_max_message_size(1024 * 1024 * 200);

    // 监听指定IP地址和端口
    m_server.listen(boost::asio::ip::make_address(ip), port);
    // 开始接收连接请求
    m_server.start_accept();

    Logger::output("RServer启动成功 监听端口: " + to_string(port), Logger::INFO);

    // 进入事件循环 开始处理连接和消息
    m_server.run();
}

void RServer::on_open(connection_hdl hdl)
{
    Logger::output("新连接已建立", Logger::INFO);
}

void RServer::on_close(connection_hdl hdl)
{
    server::connection_ptr con = m_server.get_con_from_hdl(hdl);
    for (auto &it : connectionMap)
    {
        if (it.second == con)
        {
            Logger::output("连接关闭 robotid: " + to_string(it.first), Logger::INFO);
            connectionMap.erase(it.first);
            break;
        }
    }
}

void RServer::on_message(connection_hdl hdl, server::message_ptr msg)
{
    json js = json::parse(msg->get_payload());
    string type = js["type"];

    if (type == "login")
    {
        if (js["code"].get<int>() != -1)
        {
            {
                lock_guard<mutex> lock(_connMutex);
                connectionMap[js["robotid"].get<uint32_t>()] = m_server.get_con_from_hdl(hdl);
            }
        }
        else
            Logger::output("获取robotid失败 请检查机器人是否绑定正确的robotid", Logger::ERROR);
    }
    else
    {
        {
            lock_guard<mutex> lock(_connMutex);
            msgMap[m_server.get_con_from_hdl(hdl)].push_back(js);
            _cv.notify_one();
        }
    }
}

bool RServer::isOnline(uint32_t robotid)
{
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = connectionMap.find(robotid);
        if (it != connectionMap.end())
        {
            return true;
        }
    }
    return false;
}

server::connection_ptr RServer::getConnection(uint32_t robotid)
{
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = connectionMap.find(robotid);
        if (it != connectionMap.end())
        {
            return it->second;
        }
    }
    return nullptr;
}

json RServer::getMsg(server::connection_ptr conn, const string &type)
{
    json ret;

    while (true)
    {
        {
            unique_lock<mutex> lock(_connMutex);
            _cv.wait(lock, [conn, this]
                     { return msgMap.find(conn) != msgMap.end() && !msgMap[conn].empty(); });
            for (auto it = msgMap[conn].begin(); it != msgMap[conn].end(); it++)
            {
                if (it->at("type").get<string>() == type)
                {
                    ret = *it;
                    msgMap[conn].erase(it);
                    return ret;
                }
            }
        }
        // 如果没有找到匹配的消息，继续等待
    }
    return json();
}
