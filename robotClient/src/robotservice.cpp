#include "robotservice.hpp"
#include "logger.hpp"

#include <fstream>
#include <iostream>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <dirent.h>

// 全局访问节点
RobotService *RobotService::getInstance()
{
    static RobotService instance;
    return &instance;
}

// 获取消息处理器
MsgHandler RobotService::getMsgHandler(string type)
{
    auto it = _msgHandlerMap.find(type);
    if (it == _msgHandlerMap.end())
    {
        return [=](client &m_client, connection_hdl &hdl, json &js)
        {
            Logger::output("消息类型错误 找不到对应的处理器", Logger::ERROR);
            json res_js;
            res_js["code"] = -1;
            res_js["type"] = type;
            res_js["errmsg"] = "没有对应的消息处理器";
            m_client.send(hdl, res_js.dump(), websocketpp::frame::opcode::text);
            return;
        };
    }

    return _msgHandlerMap[type];
}

RobotService::RobotService()
{
    _msgHandlerMap.insert({"getip", std::bind(&RobotService::getIP, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
    _msgHandlerMap.insert({"getdownloadedpackages", std::bind(&RobotService::getDownloadedPackages, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
    _msgHandlerMap.insert({"deploypackage", std::bind(&RobotService::deployPackage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
    _msgHandlerMap.insert({"getdeployedpackagesinfo", std::bind(&RobotService::getDeployedPackagesInfo, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
}

// 获取机器人的IP地址
void RobotService::getIP(client &m_client, connection_hdl &hdl, json &js)
{

    try
    {
        boost::asio::ip::udp::socket socket(io_context_, boost::asio::ip::udp::v4());
        socket.connect(boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string("8.8.8.8"), 53));

        std::string local_ip = socket.local_endpoint().address().to_string();

        // 构造响应的 JSON 对象
        json res_js;
        res_js["code"] = 0;
        res_js["type"] = "getip";
        res_js["IP"] = local_ip;

        m_client.send(hdl, res_js.dump(), websocketpp::frame::opcode::text);

        socket.close();
    }
    catch (const boost::system::system_error &e)
    {
        json res_js;
        res_js["code"] = -1;
        res_js["type"] = "getip";
        res_js["errmsg"] = "获取机器人IP失败: " + string(e.what());

        // 发送错误响应给客户端
        m_client.send(hdl, res_js.dump(), websocketpp::frame::opcode::text);
    }
}

void RobotService::getDownloadedPackages(client &m_client, connection_hdl &hdl, json &js)
{
    // 获取家目录
    std::string homeDir = string(getenv("HOME"));
    std::string path = homeDir + "/workspace/EMP/packages/";

    std::vector<std::string> fileNames;
    json res_js;
    res_js["type"] = "getdownloadedpackages";

    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir(path.c_str())) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            // 过滤掉当前目录"."和上级目录".."
            if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
            {
                fileNames.push_back(ent->d_name);
            }
        }
        closedir(dir);
    }
    else
    {
        Logger::output("无法获取已下载软件包列表(无法打开目录)", Logger::ERROR);
        res_js["code"] = -1;
        res_js["errmsg"] = "无法获取已下载软件包列表(无法打开目录)";
        m_client.send(hdl, res_js.dump(), websocketpp::frame::opcode::text);
        return;
    }

    res_js["code"] = 0;
    res_js["packages"] = fileNames;

    m_client.send(hdl, res_js.dump(), websocketpp::frame::opcode::text);
}

void RobotService::deployPackage(client &m_client, connection_hdl &hdl, json &js)
{
    std::string packageName = js["packagename"];

    std::string homeDir = std::string(getenv("HOME"));
    std::string file_path = homeDir + "/workspace/EMP/packages/" + packageName;

    std::string extract_path = homeDir + "/workspace/EMP/testfuck";

    // 使用系统命令解压
    std::string command = "tar -xzf " + file_path + " -C " + extract_path;
    int status = system(command.c_str());

    if (status == -1)
    {
        json res_js;
        res_js["type"] = "deploypackage";
        res_js["code"] = -1;
        res_js["errmsg"] = "部署软件包失败(解压错误)";
        m_client.send(hdl, res_js.dump(), websocketpp::frame::opcode::text);
    }
    else
    {
        json res_js;
        res_js["type"] = "deploypackage";
        res_js["code"] = 0;
        m_client.send(hdl, res_js.dump(), websocketpp::frame::opcode::text);
    }
    return;
}

void RobotService::getDeployedPackagesInfo(client &m_client, connection_hdl &hdl, json &js)
{
    std::string homeDir = std::string(getenv("HOME"));
    std::string directory_path = homeDir + "/workspace/EMP/testfuck";

    json res_js;
    res_js["type"] = "getdeployedpackagesinfo";

    vector<string> vec;

    // 打开目录
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(directory_path.c_str())) != nullptr)
    {
        while ((ent = readdir(dir)) != nullptr)
        {
            // 检查是否是子目录并且不是"."或".."
            if (ent->d_type == DT_DIR && std::string(ent->d_name) != "." && std::string(ent->d_name) != "..")
            {
                std::string sub_dir_path = directory_path + "/" + ent->d_name;
                std::string file_path = sub_dir_path + "/package_info.txt";
                std::ifstream file(file_path);

                if (file.is_open())
                {
                    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                    file.close();

                    json package_info = json::parse(content);
                    vec.push_back(package_info.dump());
                }
                else
                {
                    Logger::output("获取已部署软件包信息失败(无法打开文件):" + file_path, Logger::ERROR);
                }
            }
        }
        closedir(dir);
    }
    else
    {
        Logger::output("获取已部署软件包信息失败(无法打开目录): " + directory_path, Logger::ERROR);

        res_js["code"] = -1;
        res_js["errmsg"] = "获取已部署软件包信息失败(无法打开目录): " + directory_path;

        m_client.send(hdl, res_js.dump(), websocketpp::frame::opcode::text);
        return;
    }

    res_js["code"] = 0;
    res_js["deployedpackagesinfo"] = vec;

    m_client.send(hdl, res_js.dump(), websocketpp::frame::opcode::text);
}