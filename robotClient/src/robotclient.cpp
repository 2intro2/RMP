#include "robotclient.hpp"
#include "robotservice.hpp"
#include "logger.hpp"

#include <openssl/md5.h>
#include <functional>
#include <cstdlib>

RobotClient::RobotClient(uint32_t robotid)
{
    // 绑定机器人ID
    this->m_robotid = robotid;
    // 初始化处理文件的相关参数
    this->is_file_open = false;
    this->is_file_transfer = false;
    this->expected_file_size = 0;
    this->received_file_size = 0;
    // 初始化io_context
    m_client.init_asio(&m_io_context);

    // 注册回调函数
    m_client.set_open_handler(bind(&RobotClient::on_open, this, std::placeholders::_1));
    m_client.set_close_handler(bind(&RobotClient::on_close, this, std::placeholders::_1));
    m_client.set_fail_handler(bind(&RobotClient::on_fail, this, std::placeholders::_1));
    m_client.set_message_handler(bind(&RobotClient::on_message, this, std::placeholders::_1, std::placeholders::_2));
}

void RobotClient::start(const std::string &ip, uint16_t port)
{
    // 设置日志级别
    m_client.set_access_channels(websocketpp::log::alevel::all);

    // 关闭帧头日志
    m_client.clear_access_channels(websocketpp::log::alevel::frame_header);

    // 关闭帧负载日志
    m_client.clear_access_channels(websocketpp::log::alevel::frame_payload);

    this->m_ip = ip;
    this->m_port = port;

    websocketpp::lib::error_code ec;
    // 动态创建连接 URL
    std::string uri = "ws://" + ip + ":" + std::to_string(port);
    client::connection_ptr con = m_client.get_connection(uri, ec);

    // 检查是否成功创建连接
    if (ec)
    {
        Logger::output("无法连接到RServer: " + ec.message(), Logger::ERROR);
        return; // 连接失败，返回
    }

    // 连接到服务器
    m_client.connect(con);
    m_client.run();
}

void RobotClient::reconnect()
{
    // 等待2秒再重连
    std::this_thread::sleep_for(std::chrono::seconds(5));

    Logger::output("尝试重新连接到RServer... ", Logger::WARNING);

    start(m_ip, m_port);
}

// 新连接打开时触发的回调函数
void RobotClient::on_open(connection_hdl hdl)
{
    Logger::output("成功连接到RServer", Logger::INFO);
    json res_js;
    if (m_robotid > 0)
    {
        res_js["code"] = 0;
        res_js["type"] = "login";
        res_js["robotid"] = m_robotid;
        m_client.send(hdl, res_js.dump(), websocketpp::frame::opcode::text);
    }
    else
    {
        res_js["code"] = -1;
        res_js["type"] = "login";
        res_js["errmsg"] = "获取机器人ID失败 ";
        m_client.send(hdl, res_js.dump(), websocketpp::frame::opcode::text);
    }
}

// 连接关闭时触发的回调函数
void RobotClient::on_close(connection_hdl hdl)
{
    Logger::output("与RServer断开连接 ", Logger::ERROR);

    // 重新连接到服务器
    reconnect();
}

void RobotClient::on_fail(connection_hdl hdl)
{
    Logger::output("连接RServer失败 ", Logger::ERROR);

    // 重新连接到服务器
    reconnect();
}

// 接收到消息时触发的回调函数
void RobotClient::on_message(connection_hdl hdl, client::message_ptr msg)
{
    string payload = msg->get_payload();
    if (msg->get_opcode() == websocketpp::frame::opcode::text)
    {
        json js = json::parse(payload);
        string type = js["type"].get<string>();
        if (type == "sendpackage")
        {
            // 处理文件信息消息
            string file_name = js["packagename"].get<std::string>();
            // 获取家目录
            string homeDir(getenv("HOME"));
            string file_path = homeDir + "/workspace/EMP/packages/" + file_name;

            // 检查文件是否存在
            if (!isFileExists(file_path))
            {
                json res_js;
                res_js["code"] = 0;
                res_js["type"] = "isready";
                m_client.send(hdl, res_js.dump(), websocketpp::frame::opcode::text);
            }
            else
            {
                // 计算MD5校验和
                string md5sum = getMD5SUM(file_path);
                if (md5sum == js["md5sum"].get<string>())
                {
                    json res_js;
                    res_js["code"] = -1;
                    res_js["type"] = "isready";
                    res_js["errmsg"] = "软件包已下载 请勿重复下载";
                    m_client.send(hdl, res_js.dump(), websocketpp::frame::opcode::text);
                    return;
                }
                else
                {
                    json res_js;
                    res_js["code"] = 0;
                    res_js["type"] = "isready";
                    m_client.send(hdl, res_js.dump(), websocketpp::frame::opcode::text);
                }
            }

            expected_file_size = js["filesize"].get<size_t>();
            received_file_size = 0;

            double expected_file_size_mb = static_cast<double>(expected_file_size) / (1024 * 1024);
            Logger::output("开始接收软件包: " + file_name + " 文件大小(MB): " + to_string(expected_file_size_mb), Logger::INFO);

            file.open(file_path, std::ios::binary);
            if (!file.is_open())
            {
                Logger::output("软件包数据写入失败(无法打开文件) " + file_path, Logger::ERROR);

                json res_js;
                res_js["code"] = -1;
                res_js["type"] = "sendpackage";
                res_js["errmsg"] = "机器人端无法打开文件写入文件数据";
                m_client.send(hdl, res_js.dump(), websocketpp::frame::opcode::text);
                return;
            }
            is_file_open = true;
            is_file_transfer = true;
        }
        else if (type == "transfercompelete")
        {
            // 处理文件传输完成消息
            if (is_file_transfer)
            {
                Logger::output("软件包数据全部接收 正在写入..", Logger::INFO);
                is_file_transfer = false;
            }
            else
            {
                Logger::output("软件包数据全部接收 但未能写入", Logger::WARNING);
            }
            return;
        }
        else
        {
            // 获取消息处理器
            auto _msgHandler = RobotService::getInstance()->getMsgHandler(type);
            _msgHandler(m_client, hdl, js);
        }
    }
    else if (msg->get_opcode() == websocketpp::frame::opcode::binary)
    {
        if (is_file_open)
        {
            file.write(payload.c_str(), payload.size());
            received_file_size += payload.size();

            Logger::output(":)已接收 " + to_string(received_file_size) + " / " + to_string(expected_file_size) + "字节", Logger::INFO);

            if (received_file_size >= expected_file_size)
            {
                Logger::output(":)软件包数据已经全部写入", Logger::INFO);

                json res_js;
                res_js["code"] = 0;
                res_js["type"] = "sendpackage";
                m_client.send(hdl, res_js.dump(), websocketpp::frame::opcode::text);

                file.close();
                is_file_open = false;
            }
        }
    }
}

bool RobotClient::isFileExists(const std::string &file_path)
{
    struct stat buffer;
    return (stat(file_path.c_str(), &buffer) == 0); // 如果文件存在返回true
}

std::string RobotClient::getMD5SUM(const std::string &file_path)
{
    unsigned char result[MD5_DIGEST_LENGTH];
    MD5_CTX md5_ctx;
    MD5_Init(&md5_ctx);

    ifstream file(file_path, std::ios::binary);
    if (!file)
    {
        Logger::output("文件无法打开 无法计算MD5校验和: " + file_path, Logger::ERROR);
        return "";
    }

    char buffer[4096];
    while (file.read(buffer, sizeof(buffer)))
    {
        MD5_Update(&md5_ctx, buffer, file.gcount());
    }
    MD5_Update(&md5_ctx, buffer, file.gcount());
    MD5_Final(result, &md5_ctx);

    std::stringstream md5_string;
    for (const auto &byte : result)
    {
        md5_string << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    }
    return md5_string.str();
}