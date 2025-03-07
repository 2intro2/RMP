#ifndef USER_HPP
#define USER_HPP

#include <string>

using namespace std;
class User
{
public:
    User(string id="",string name="",string password="")
    {
        this->id=id;
        this->name=name;
        this->password=password;
    }

    void setId(string id) { this->id=id; }
    void setName(string name) { this->name=name; }
    void setPwd(string pwd) { this->password=pwd; }


    string getId() { return this->id; }
    string getName() { return this->name; }
    string getPwd() { return this->password; }

protected:
    string id;
    string name;
    string password;

};

#endif