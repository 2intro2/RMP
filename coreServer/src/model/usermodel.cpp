#include "usermodel.hpp"
#include "db.hpp"

//User表中进行查询
User UserModel::query(string userid)
{
    //1.组合sql语句
    char sql[1024]="";
    sprintf(sql,"select * from User where userid='%s'",userid.c_str());

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
                User user;
                user.setId(row[0]);
                user.setName(row[1]);
                user.setPwd(row[2]);

                //释放资源
                mysql_free_result(res);
                return user;
            }
        }
    }

    //查询异常
    return User();
}
