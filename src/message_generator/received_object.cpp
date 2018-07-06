#include "received_object.h"
#include <sstream>

namespace fipa {
namespace acl {

std::string ReceivedObject::toString(size_t indent) const
{
    std::string hspace(indent,' ');
    std::stringstream ss;
    ss << hspace << "ReceivedObject:" << std::endl;
    ss << hspace << "    by: " << mBy << std::endl;
    ss << hspace << "    from: " << mFrom << std::endl;
    ss << hspace << "    date: " << mDate.toString() << std::endl;
    ss << hspace << "    id: " << mId << std::endl;
    ss << hspace << "    via: " << mVia << std::endl;
    ss << hspace << "    user-defined-parameters:" << std::endl;
    std::vector<UserdefParam>::const_iterator cit = mUserdefinedParameters.begin();
    for(; cit != mUserdefinedParameters.end(); ++cit)
    {
        ss << hspace << "        " << cit->toString() << std::endl;
    }

    return ss.str();
}

} // end namespace acl
} // end namespace fipa
