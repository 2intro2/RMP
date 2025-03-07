#ifndef ROBOTLIST_HPP
#define ROBOTLIST_HPP

#include <string>
using namespace std;
class RobotList
{
public:
    RobotList(int robotid=-1)
    {   
        this->robotid=robotid;
    }

    void setRobotId(int id) { this->robotid=id; }

    int getRobotId() { return this->robotid; }

protected:
    int robotid;
};

#endif

/*
create table RobotList (
    userid string,
    robotid int,
    PRIMARY KEY (userid, robotid)
);
*/