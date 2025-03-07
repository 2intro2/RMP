#include "robotinfomodel.hpp"
#include "db.hpp"

//User表中进行查询
RobotInfo RobotInfoModel::query(int robotid)
{
    //1.组合sql语句
    char sql[1024]="";
    sprintf(sql,"select * from RobotInfo where robotid=%d",robotid);

    //2.连接数据库进行操作
    MySQL mysql;
    if(mysql.connect())
    {
        MYSQL_RES* res=mysql.query(sql);
        //查询操作成功
        if(res!=nullptr)
        {
            MYSQL_ROW row=mysql_fetch_row(res);
            //获取查询结果成功
            if(row!=nullptr)
            {
                RobotInfo _robotinfo;
                _robotinfo.setRobotId(atoi(row[0]));
                _robotinfo.setModel(row[1]);
                _robotinfo.setName(row[2]);

                //释放资源
                mysql_free_result(res);
                return _robotinfo;
            }
        }
    }

    //查询异常
    return RobotInfo();
}
