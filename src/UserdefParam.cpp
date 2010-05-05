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
std::string UserdefParam::getValue() {return value;}

void UserdefParam::setValue(std::string val) {value = val;}

std::string UserdefParam::getName() {return name;}

void UserdefParam::setName (std::string nam) {name = nam;}

}//end of acl namespace

}// end of fipa namespace

