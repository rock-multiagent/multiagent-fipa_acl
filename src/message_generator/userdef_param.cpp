/**
 *
 * \file userdef_param.cpp
 * \author Mircea Cretu Stancu
 * \author Thomas Roehr, thomas.roehr@dfki.de
 * \brief Implements the general User-defined parameters, which are present throughout the fipa specifications(FIPA at http://www.fipa.org).
 *
 */

#include "userdef_param.h"
#include "acl_message.h"
#include <stdexcept>

namespace fipa {
namespace acl {

UserdefParam::UserdefParam()
    : mName()
    , mValue()
{
}

UserdefParam::UserdefParam(const std::string& name, const std::string& value)
    : mName(name)
    , mValue(value)
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

void UserdefParam::setName(const std::string& name)
{
    if ( (name.find_first_of(illegalWordChars) != std::string::npos) || (illegalWordStart.find_first_of(name.c_str()[0]) != std::string::npos) )
    {
        throw std::runtime_error("Illegal name for userdefined-parameter");
    }
    mName = name;
}

std::string UserdefParam::toString() const
{
    return mName + ": " + mValue;
}

}//end of acl namespace
}// end of fipa namespace

