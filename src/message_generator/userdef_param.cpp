/**
 *
 * \file userdef_param.cpp
 * \author Mircea Cretu Stancu
 * \brief Implements the general User-defined parameters, which are present throughout the fipa specifications(FIPA at http://www.fipa.org).
 * 
 * \version 1.0
 *  
 */

#include "userdef_param.h"
#include "acl_message.h"

namespace fipa {

namespace acl {
/*  
bool operator== (UserdefParam &a, UserdefParam &b)
{
    if (a.getName().compare(b.getName()))
        return false;
    if (a.getValue().compare(b.getValue()))
        return false;
}
*/
UserdefParam::UserdefParam()
{
}

UserdefParam::UserdefParam(const std::string& _name) : name(_name)
{
}

UserdefParam::UserdefParam(const UserdefParam &copy)
{
    name = copy.getName();
    value = copy.getValue();
}

UserdefParam& UserdefParam::operator=(const UserdefParam &copy)
{
    if (this != &copy) {
    name.clear();
    value.clear();
    name = copy.getName();
    value = copy.getValue(); }
    return *this;
}

std::string UserdefParam::getValue() const {return value;}

void UserdefParam::setValue(std::string val) {value = val;}

std::string UserdefParam::getName() const {return name;}

int UserdefParam::setName (std::string nam) 
{
    if ( (nam.find_first_of(illegalWordChars) != -1) || (illegalWordStart.find_first_of(nam.c_str()[0]) != -1) )
    return 1;
    name = nam; return 0;
}


bool operator== (const UserdefParam &a,const UserdefParam &b)
{
    if (a.getName().compare(b.getName()))
        return false;
    if (a.getValue().compare(b.getValue()))
        return false;
    return true;
}


}//end of acl namespace

}// end of fipa namespace

