#ifndef ROBOTINFO_HPP
#define ROBOTINFO_HPP

#include <string>
using namespace std;
class RobotInfo
{
public:
    RobotInfo(int robotid = -1, string model = "", string name = "")
    {
        this->robotid = robotid;
        this->model = model;
        this->name = name;
    }

    void setRobotId(int id) { this->robotid = id; }
    void setModel(string model) { this->model = model; }
    void setName(string name) { this->name = name; }

    int getRobotId() { return this->robotid; }
    string getModel() { return this->model; }
    string getName() { return this->name; }

protected:
    int robotid;
    string model;
    string name;
};

#endif

/*
create table RobotInfo(
    robotid int  PRIMARY KEY,
    model varchar(30),
    name varchar(50)
);
*/