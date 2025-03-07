#ifndef USERMODEL_HPP
#define USERMODEL_HPP

#include <string>

#include "user.hpp"

using namespace std;

class UserModel
{
public:
    //User表中进行查询
    User query(string userid);
private: 
};

#endif