#include "date_time.h"
#include <base-logging/Logging.hpp>

namespace fipa {
namespace acl {
 
const std::string DateTime::defaultFormat = "%Y-%m-%dT%H:%M:%S";

std::string DateTime::toString() const
{
 char buffer[512];
     // %Z missing so far
     strftime(buffer,512,defaultFormat.c_str(), &dateTime);

     std::string formattedOutput;
     if(relative != '\0')
     {
     	formattedOutput += relative;
     }
     formattedOutput += std::string(buffer);
     if(timezone != '\0')
     {
         formattedOutput += timezone;
     }
     return formattedOutput;
}

base::Time DateTime::toTime() const
{
    // Tell make to check whether daylight saving is in effect
    // http://pubs.opengroup.org/onlinepubs/007904975/functions/strptime.html
    Time timeTmp = dateTime;
    timeTmp.tm_isdst = -1;
    time_t time = mktime(dynamic_cast<std::tm*>(&timeTmp));
    return base::Time::fromMilliseconds(time*static_cast<uint64_t>(base::Time::Milliseconds) + timeTmp.tm_msec);
}

}
}
