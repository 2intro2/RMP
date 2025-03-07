#ifndef ROBOTLISTMODEL_HPP
#define ROBOTLISTMODEL_HPP  

#include <vector>
#include "robotlist.hpp"
using namespace std;

class RobotListModel
{
public:
    //RobotList表中进行查询
    vector<RobotList> query(string userid);
private:
};

#endif