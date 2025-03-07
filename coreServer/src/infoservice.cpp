#include <iostream>
#include <fstream>
#include <filesystem>
#include <curl/curl.h>

#include "infoservice.hpp"
#include "rserver.hpp"
#include "logger.hpp"

// 全局访问节点
InfoService *InfoService::getInstance()
{
    static InfoService instance;
    return &instance;
}

InfoService::InfoService()
{
    // 初始化包MD5校验和映射表
    setPackageMd5();

    // 将消息和对应的处理函数进行绑定
    _msgHandlerMap.insert({"login", std::bind(&InfoService::Login, this, std::placeholders::_1, std::placeholders::_2)});
    _msgHandlerMap.insert({"getrobotlist", std::bind(&InfoService::getRobotList, this, std::placeholders::_1, std::placeholders::_2)});
    _msgHandlerMap.insert({"getrobotinfo", std::bind(&InfoService::getRobotInfo, this, std::placeholders::_1, std::placeholders::_2)});
    _msgHandlerMap.insert({"getpackages", std::bind(&InfoService::getPackages, this, std::placeholders::_1, std::placeholders::_2)});

    _msgHandlerMap.insert({"getonlinestatus", std::bind(&InfoService::getOnlineStatus, this, std::placeholders::_1, std::placeholders::_2)});
    _msgHandlerMap.insert({"getallonlinestatus", std::bind(&InfoService::getAllOnlineStatus, this, std::placeholders::_1, std::placeholders::_2)});
    _msgHandlerMap.insert({"sendpackage", std::bind(&InfoService::sendPackage, this, std::placeholders::_1, std::placeholders::_2)});
    _msgHandlerMap.insert({"bulksendpackage", std::bind(&InfoService::bulksendPackage, this, std::placeholders::_1, std::placeholders::_2)});
    _msgHandlerMap.insert({"getdownloadedpackages", std::bind(&InfoService::getDownloadedPackages, this, std::placeholders::_1, std::placeholders::_2)});
    _msgHandlerMap.insert({"deploypackage", std::bind(&InfoService::deployPackage, this, std::placeholders::_1, std::placeholders::_2)});
    _msgHandlerMap.insert({"getdeployedpackagesinfo", std::bind(&InfoService::getDeployedPackagesInfo, this, std::placeholders::_1, std::placeholders::_2)});
}

// 获取特定消息对应的处理函数对象
MsgHandler InfoService::getHandler(string key)
{
    // 错误日志 msgid没有对应的事件处理器(返回默认的空处理器)
    auto it = _msgHandlerMap.find(key);
    if (it == _msgHandlerMap.end())
    {
        return [=](httplib::Response &res, json &js)
        {
            Logger::output("消息类型错误 找不到对应的处理器", Logger::ERROR);
            json res_js;
            res_js["code"] = -1;
            res_js["errmsg"] = "key error! handler not found!";
            res.body = res_js.dump();
            return;
        };
    }

    return _msgHandlerMap[key];
}

void InfoService::setPackageMd5()
{
    string configPath = "../packages/p.config";
    ifstream configFileStream(configPath);

    if (!configFileStream.is_open())
    {
        Logger::output("无法打开配置文件", Logger::ERROR);
    }

    string line;

    while (getline(configFileStream, line))
    {
        istringstream iss(line);
        string filename, md5sum;
        if (iss >> filename >> md5sum)
        {
            _packageMd5Map[filename] = md5sum;
        }
    }

    configFileStream.close();

    // 输出每个文件名和对应的MD5校验和
    // for (const auto &entry : _packageMd5Map)
    // {
    //     std::cout << "File: " << entry.first << " MD5sum: " << entry.second << std::endl;
    // }
}

void InfoService::Login(httplib::Response &res, json &js)
{

    string userid = js["userid"].get<string>();
    string password = js["password"].get<string>();

    Logger::output("登录请求 userid: " + userid, Logger::INFO);

    json res_js;

    User user = _userModel.query(userid);
    if (user.getId() == userid && user.getPwd() == password)
    {
        // 登录成功
        res_js["code"] = 0;
        res_js["userid"] = user.getId();
        res_js["username"] = user.getName();
        res.body = res_js.dump();
    }
    else
    {
        // 登录失败
        res_js["code"] = -1;
        res_js["errmsg"] = "登录失败! 密码错误或者用户不存在!";
        res.body = res_js.dump();
    }
}

void InfoService::getOnlineStatus(httplib::Response &res, json &js)
{
    uint32_t robotid = js["robotid"].get<uint32_t>();

    Logger::output("获取机器人在线状态请求 robotid: " + to_string(robotid), Logger::INFO);

    json res_js;
    RServer::getInstance()->isOnline(robotid) ? res_js["code"] = 0 : res_js["code"] = -1;
    res.body = res_js.dump();
}

void InfoService::getAllOnlineStatus(httplib::Response &res, json &js)
{
    string userid = js["userid"].get<string>();
    vector<uint32_t> robotids = js["robotids"].get<vector<uint32_t>>();

    Logger::output("获取所有机器人在线状态请求 userid: " + userid, Logger::INFO);

    json res_js;
    vector<string> vec;
    for (auto &robotid : robotids)
    {
        json tmp;
        tmp["robotid"] = robotid;
        tmp["code"] = RServer::getInstance()->isOnline(robotid) ? 0 : -1;
        vec.push_back(tmp.dump());
    }

    res_js["code"] = 0;
    res_js["result"] = vec;
    res.body = res_js.dump();
}

// 获取某个用户拥有机器人列表
void InfoService::getRobotList(httplib::Response &res, json &js)
{

    string userid = js["userid"].get<string>();

    Logger::output("获取机器人列表请求 userid: " + userid, Logger::INFO);

    json res_js;

    vector<RobotList> vec = _robotListModel.query(userid);
    if (!vec.empty())
    {

        vector<string> vec2;

        for (auto &x : vec)
        {
            json tmp;
            tmp["robotid"] = x.getRobotId();
            vec2.push_back(tmp.dump());
        }

        res_js["code"] = 0;
        res_js["robotlist"] = vec2;
        res.body = res_js.dump();
    }
    else
    {
        res_js["code"] = -1;
        res_js["errmsg"] = "RobotList is empty!";
        res.body = res_js.dump();
    }
}

void InfoService::getRobotInfo(httplib::Response &res, json &js)
{
    int robotid = js["robotid"].get<int>();

    Logger::output("获取机器人信息请求 robotid: " + to_string(robotid), Logger::INFO);

    json res_js;

    RobotInfo _robotInfo = _robotInfoModel.query(robotid);
    if (_robotInfo.getRobotId() == robotid)
    {
        res_js["code"] = 0;
        res_js["model"] = _robotInfo.getModel();
        res_js["name"] = _robotInfo.getName();
    }
    else
    {
        res_js["code"] = -1;
        res_js["errmsg"] = "机器人ID不存在！";
        res.body = res_js.dump();
        return;
    }

    server::connection_ptr con = RServer::getInstance()->getConnection(robotid);
    if (!con)
    {
        res_js["IP"] = "获取机器人IP失败 请检查连接状态";
        res.body = res_js.dump();
        return;
    }

    // 发送文件信息
    json info;
    info["type"] = "getip";
    con->send(info.dump(), websocketpp::frame::opcode::text);

    json ret = RServer::getInstance()->getMsg(con, "getip");
    if (ret["code"] == 0)
    {
        res_js["IP"] = ret["IP"].get<string>();
    }
    else
    {
        res_js["IP"] = ret["errmsg"].get<string>();
    }

    res.body = res_js.dump();
}

void InfoService::getPackages(httplib::Response &res, json &js)
{

    Logger::output("获取可下载软件包信息请求", Logger::INFO);

    json res_js;

    vector<string> vec = _packagesModel.query();
    if (!vec.empty())
    {
        res_js["code"] = 0;
        res_js["packages"] = vec;
        res.body = res_js.dump();
    }
    else
    {
        res_js["code"] = -1;
        res_js["errmsg"] = "Packages is empty!";
        res.body = res_js.dump();
    }
}

void InfoService::sendPackage(httplib::Response &res, json &js)
{
    uint32_t robotid = js["robotid"].get<uint32_t>();
    string packagename = js["packagename"].get<string>();

    Logger::output("发送软件包请求", Logger::INFO);

    json res_js;

    std::string file_path = "../packages/" + packagename;
    std::ifstream file(file_path, std::ios::binary);
    if (!file)
    {
        Logger::output("指定软件包不存在", Logger::ERROR);

        res_js["code"] = -1;
        res_js["errmsg"] = "服务器无法找到该文件!";
        res.body = res_js.dump();

        return;
    }

    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    char buffer[1024];
    server::connection_ptr con = RServer::getInstance()->getConnection(robotid);
    if (!con)
    {
        Logger::output("无法获取机器人连接 robotid: " + to_string(robotid), Logger::ERROR);

        res_js["code"] = -1;
        res_js["errmsg"] = "机器人离线 无法下载软件包!";
        res.body = res_js.dump();

        return;
    }
    // 发送文件信息
    json file_info;
    file_info["type"] = "sendpackage";
    file_info["packagename"] = packagename;
    file_info["filesize"] = file_size;
    file_info["md5sum"] = _packageMd5Map[packagename];
    con->send(file_info.dump(), websocketpp::frame::opcode::text);

    json ret_1 = RServer::getInstance()->getMsg(con, "isready");
    if (ret_1["code"] == -1)
    {
        Logger::output("软件包已存在 不会重复下载: " + packagename, Logger::WARNING);
        res.body = ret_1.dump();
        return;
    }

    // 发送文件数据
    while (file.read(buffer, sizeof(buffer)))
    {
        con->send(buffer, sizeof(buffer), websocketpp::frame::opcode::binary);
    }
    // 发送剩余的数据（如果有的话）
    size_t remaining = file.gcount();
    if (remaining > 0)
    {
        con->send(buffer, remaining, websocketpp::frame::opcode::binary);
    }

    // 发送结束消息
    json end_info;
    end_info["type"] = "transfercompelete";
    con->send(end_info.dump(), websocketpp::frame::opcode::text);

    file.close();

    json ret_2 = RServer::getInstance()->getMsg(con, "sendpackage");
    res.body = ret_2.dump();
}

// 批量发送软件包资源
void InfoService::bulksendPackage(httplib::Response &res, json &js)
{
    vector<uint32_t> robotids = js["robotids"].get<vector<uint32_t>>();
    string packagename = js["packagename"].get<string>();

    for (auto &robotid : robotids)
    {
        Logger::output("robotid: " + to_string(robotid), Logger::DEBUG);
    }

    Logger::output("批量发送软件包请求", Logger::INFO);

    json res_js;
    vector<string> vec;

    std::string file_path = "../packages/" + packagename;
    std::ifstream file(file_path, std::ios::binary);
    if (!file)
    {
        Logger::output("指定软件包不存在", Logger::ERROR);

        res_js["code"] = -1;
        res_js["errmsg"] = "服务器无法找到该软件包!";
        res.body = res_js.dump();

        return;
    }

    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();

    for (auto &robotid : robotids)
    {
        char buffer[1024];
        file.seekg(0, std::ios::beg);

        server::connection_ptr con = RServer::getInstance()->getConnection(robotid);
        if (!con)
        {
            Logger::output("无法获取机器人连接 robotid: " + to_string(robotid), Logger::ERROR);

            json tmp;
            tmp["code"] = -1;
            tmp["robotid"] = robotid;
            tmp["errmsg"] = "机器人离线 无法下载软件包!";
            vec.push_back(tmp.dump());

            continue;
        }
        // 发送文件信息
        json file_info;
        file_info["type"] = "sendpackage";
        file_info["packagename"] = packagename;
        file_info["filesize"] = file_size;
        file_info["md5sum"] = _packageMd5Map[packagename];

        con->send(file_info.dump(), websocketpp::frame::opcode::text);

        json ret_1 = RServer::getInstance()->getMsg(con, "isready");
        if (ret_1["code"] == -1)
        {
            Logger::output("软件包已存在 不会重复下载: " + packagename, Logger::WARNING);
            ret_1["robotid"] = robotid;
            vec.push_back(ret_1.dump());
            continue;
        }

        // 发送文件数据
        while (file.read(buffer, sizeof(buffer)))
        {
            con->send(buffer, sizeof(buffer), websocketpp::frame::opcode::binary);
        }
        // 发送剩余的数据（如果有的话）
        size_t remaining = file.gcount();
        if (remaining > 0)
        {
            con->send(buffer, remaining, websocketpp::frame::opcode::binary);
        }

        // 发送结束消息
        json end_info;
        end_info["type"] = "transfercompelete";
        con->send(end_info.dump(), websocketpp::frame::opcode::text);

        json ret_2 = RServer::getInstance()->getMsg(con, "sendpackage");
        ret_2["robotid"] = robotid;
        vec.push_back(ret_2.dump());

        // 清除文件流的状态标志
        file.clear();
    }

    file.close();

    res_js["code"] = 0;
    res_js["result"] = vec;
    res.body = res_js.dump();
}

void InfoService::getDownloadedPackages(httplib::Response &res, json &js)
{
    uint32_t robotid = js["robotid"].get<uint32_t>();
    Logger::output("获取已下载软件包请求 robotid: " + to_string(robotid), Logger::INFO);

    server::connection_ptr con = RServer::getInstance()->getConnection(robotid);
    if (!con)
    {
        Logger::output("无法获取机器人连接 robotid: " + to_string(robotid), Logger::ERROR);
        json res_js;
        res_js["code"] = -1;
        res_js["errmsg"] = "机器人离线 无法获取已下载软件包列表!";
        res.body = res_js.dump();
        return;
    }

    json info;
    info["type"] = "getdownloadedpackages";
    con->send(info.dump(), websocketpp::frame::opcode::text);

    json ret = RServer::getInstance()->getMsg(con, "getdownloadedpackages");
    res.body = ret.dump();
}

void InfoService::deployPackage(httplib::Response &res, json &js)
{
    uint32_t robotid = js["robotid"].get<uint32_t>();
    Logger::output("部署软件包请求 robotid: " + to_string(robotid), Logger::INFO);

    server::connection_ptr con = RServer::getInstance()->getConnection(robotid);
    if (!con)
    {
        Logger::output("无法获取机器人连接 robotid: " + to_string(robotid), Logger::ERROR);
        json res_js;
        res_js["code"] = -1;
        res_js["errmsg"] = "机器人离线 无法进行部署!";
        res.body = res_js.dump();
        return;
    }

    json info;
    info["type"] = "deploypackage";
    info["packagename"] = js["packagename"].get<string>();
    con->send(info.dump(), websocketpp::frame::opcode::text);

    json ret = RServer::getInstance()->getMsg(con, "deploypackage");
    res.body = ret.dump();
}

void InfoService::getDeployedPackagesInfo(httplib::Response &res, json &js)
{
    uint32_t robotid = js["robotid"].get<uint32_t>();
    Logger::output("获取已部署软件包信息请求 robotid: " + to_string(robotid), Logger::INFO);

    server::connection_ptr con = RServer::getInstance()->getConnection(robotid);
    if (!con)
    {
        Logger::output("无法获取机器人连接 robotid: " + to_string(robotid), Logger::ERROR);
        json res_js;
        res_js["code"] = -1;
        res_js["errmsg"] = "机器人离线 无法获取已部署软件包信息!";
        res.body = res_js.dump();
        return;
    }

    json info;
    info["type"] = "getdeployedpackagesinfo";
    con->send(info.dump(), websocketpp::frame::opcode::text);

    json ret = RServer::getInstance()->getMsg(con, "getdeployedpackagesinfo");
    res.body = ret.dump();
}