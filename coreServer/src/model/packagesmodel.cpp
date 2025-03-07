#include "packagesmodel.hpp"
#include "db.hpp"
#include "json.hpp"
using json = nlohmann::json;

// User表中进行查询
vector<string> PackagesModel::query()
{
    // 1.组合sql语句
    char sql[1024] = "";
    sprintf(sql, "select * from PackageList");

    // 2.连接数据库进行操作
    vector<string> vec;
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        // 查询操作成功
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                json js;
                js["id"] = atoi(row[0]);
                js["name"] = row[1];
                vec.push_back(js.dump());
            }
            mysql_free_result(res);
        }
    }
    return vec;
}