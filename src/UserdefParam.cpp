/**
 *
 * \file UserdefParam.cpp
 * \author Mircea Cretu Stancu
 * \brief Implements the general User-defined parameters, which are present throughout the fipa specifications(FIPA at http://www.fipa.org).
 * 
 * \version 1.0
 *  
 */

#include "UserdefParam.h"

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

void UserdefParam::setName (std::string nam) {name = nam;}


bool operator== (const UserdefParam &a,const UserdefParam &b)
{
    if (a.getName().compare(b.getName()))
        return false;
    if (a.getValue().compare(b.getValue()))
        return false;
}


}//end of acl namespace

}// end of fipa namespace

