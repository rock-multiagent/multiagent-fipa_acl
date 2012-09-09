#include "date_time.h"
#include <base/logging.h>

namespace fipa {
namespace acl {
 
const std::string DateTime::defaultFormat = "%Y-%m-%dT%H:%M:%S";

std::string DateTime::toString()
{
 char buffer[512];
     // %Z missing so far
     strftime(buffer,512,defaultFormat.c_str(), &dateTime);

     std::string formattedOutput;
     formattedOutput += relative;
     formattedOutput += std::string(buffer);
     formattedOutput += timezone;

     return formattedOutput;
}

base::Time DateTime::toTime()
{
    // Tell make to check whether daylight saving is in effect
    // http://pubs.opengroup.org/onlinepubs/007904975/functions/strptime.html
    dateTime.tm_isdst = -1;
    time_t time = mktime(dynamic_cast<std::tm*>(&dateTime));
    return base::Time::fromMilliseconds(time*static_cast<uint64_t>(base::Time::Milliseconds) + dateTime.tm_msec);
}

}
}
