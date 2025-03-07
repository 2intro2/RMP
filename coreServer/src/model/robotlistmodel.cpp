#include "robotlistmodel.hpp"
#include "db.hpp"

//User表中进行查询
vector<RobotList> RobotListModel::query(string userid)
{
    //1.组合sql语句
    char sql[1024]="";
    sprintf(sql,"select robotid from RobotList where userid='%s'",userid.c_str());

    //2.连接数据库进行操作
    vector<RobotList> robotVec;
    MySQL mysql;
    if(mysql.connect())
    {
        MYSQL_RES* res=mysql.query(sql);
        //查询操作成功
        if(res!=nullptr)
        {
            MYSQL_ROW row;
            while((row=mysql_fetch_row(res))!=nullptr)
            {   
                RobotList _robot;
                _robot.setRobotId(atoi(row[0]));
                robotVec.push_back(_robot);
            }
            mysql_free_result(res);
        }
    }
    return robotVec;
}