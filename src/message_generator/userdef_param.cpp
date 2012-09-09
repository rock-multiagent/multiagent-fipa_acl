/**
 *
 * \file userdef_param.cpp
 * \author Mircea Cretu Stancu, Thomas Roehr
 * \brief Implements the general User-defined parameters, which are present throughout the fipa specifications(FIPA at http://www.fipa.org).
 * 
 * \version 1.0
 *  
 */

#include "userdef_param.h"
#include "acl_message.h"

namespace fipa {
namespace acl {

UserdefParam::UserdefParam()
    : mName()
    , mValue()
{
}

UserdefParam::UserdefParam(const std::string& name) 
    : mName(name)
    , mValue()
{
}

UserdefParam& UserdefParam::operator=(const UserdefParam& other)
{
    if(this != &other)
    {
        mName = other.mName;
        mValue = other.mValue;
    }

    return *this;
}

bool UserdefParam::operator==(const UserdefParam& other) const
{
    return mName == other.mName && mValue == other.mValue;
}

bool UserdefParam::setName(const std::string& name) 
{
    if ( (name.find_first_of(illegalWordChars) != std::string::npos) || (illegalWordStart.find_first_of(name.c_str()[0]) != std::string::npos) )
    {
        return false;
    } else {
        mName = name;
        return true;
    }
}

}//end of acl namespace
}// end of fipa namespace

