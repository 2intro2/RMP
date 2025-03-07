#include "db.hpp"
#include <iostream>

// 数据库配置信息
static string server = "127.0.0.1";
static string user = "root";
static string password = "123456";
static string dbname = "Galaxea";

// 分配数据库连接资源
MySQL::MySQL()
{
    _conn = mysql_init(nullptr);
}

// 释放数据库连接资源
MySQL::~MySQL()
{
    if (_conn != nullptr)
        mysql_close(_conn);
}

// 连接数据库
bool MySQL::connect()
{
    MYSQL *p = mysql_real_connect(_conn, server.c_str(), user.c_str(),
                                  password.c_str(), dbname.c_str(), 3306, nullptr, 0);
    // c/c++代码默认ASCII编码 需要进行设置 否则中文乱码
    if (p != nullptr)
    {

        // 设置字符编码 非常重要 否则中文将插入不了
        // 注意MySQL字符编码也需要同步进行设置
        mysql_query(_conn, "set names utf8mb4");
        std::cerr << "connect MySQL success!" << std::endl;
    }
    else
    {
        std::cerr << "connect MySQL fail!" << std::endl;
    }
    return p;
}

// 更新操作
bool MySQL::update(string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        std::cerr << __FILE__ << ":" << __LINE__ << ":"
                  << sql << "更新失败:" << std::endl;
        return false;
    }
    return true;
}

// 查询操作
MYSQL_RES *MySQL::query(string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        std::cerr << __FILE__ << ":" << __LINE__ << ":"
                  << sql << "查询失败!" << std::endl;
        return nullptr;
    }
    return mysql_use_result(_conn);
}

// 获取连接
MYSQL *MySQL::getConnection()
{
    return this->_conn;
}