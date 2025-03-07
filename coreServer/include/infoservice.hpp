#ifndef INFOSERVICE_HPP
#define INFOSERVICE_HPP

#include <unordered_map>
#include <functional>
#include <string>

#include "json.hpp"
#include "httplib.h"

#include "usermodel.hpp"
#include "robotlistmodel.hpp"
#include "robotinfomodel.hpp"
#include "packagesmodel.hpp"

using namespace std;
using json = nlohmann::json;
// 表示处理消息的回调方法的类型
using MsgHandler = std::function<void(httplib::Response &, json &)>;

class InfoService
{
public:
    // 全局访问节点
    static InfoService *getInstance();
    // 获取特定消息对应的处理器
    MsgHandler getHandler(string key);
    // 读取配置文件设置软件包的md5校验
    void setPackageMd5();

    // 处理与用户相关的请求
    // 处理登陆业务
    void Login(httplib::Response &res, json &js);
    // 获取某个用户拥有的机器人列表
    void getRobotList(httplib::Response &res, json &js);
    // 获取某个机器人的详细信息
    void getRobotInfo(httplib::Response &res, json &js);
    // 获取可下载的软件包列表
    void getPackages(httplib::Response &res, json &js);

    // 处理与机器人相关的请求
    // 获取机器人在线状态
    void getOnlineStatus(httplib::Response &res, json &js);
    // 获取所有机器人的在线状态
    void getAllOnlineStatus(httplib::Response &res, json &js);
    // 发送软件包资源
    void sendPackage(httplib::Response &res, json &js);
    // 批量发送软件包资源
    void bulksendPackage(httplib::Response &res, json &js);
    // 获取机器人已下载软件包列表
    void getDownloadedPackages(httplib::Response &res, json &js);
    // 部署软件包
    void deployPackage(httplib::Response &res, json &js);
    // 获取已部署软件包信息
    void getDeployedPackagesInfo(httplib::Response &res, json &js);

private:
    // 使用哈希表存储消息id与其对应的处理方法
    unordered_map<string, MsgHandler> _msgHandlerMap;
    // 存储软件包的md5校验
    unordered_map<string, string> _packageMd5Map;

    // 表的操作接口
    UserModel _userModel;
    RobotListModel _robotListModel;
    RobotInfoModel _robotInfoModel;
    PackagesModel _packagesModel;

    // 单例模式
    InfoService();
    InfoService(const InfoService &rhs) = delete;
    const InfoService &operator=(const InfoService &rhs) = delete;
};

#endif