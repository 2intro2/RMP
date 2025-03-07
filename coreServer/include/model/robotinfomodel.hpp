#ifndef ROBOTINFOMODEL_HPP
#define ROBOTINFOMODEL_HPP

#include <string>

#include "robotinfo.hpp"

using namespace std;

class RobotInfoModel
{
public:
    //User表中进行查询
    RobotInfo query(int robotid);
private: 
};

#endif