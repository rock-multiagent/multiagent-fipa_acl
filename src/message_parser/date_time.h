#ifndef __FIPAACL_MESSAGEPARSER_DATETIME_H_
#define __FIPAACL_MESSAGEPARSER_DATETIME_H_

#include <stdio.h>
#include <string>
#include <ctime>
#include <base/time.h>

namespace fipa { namespace acl {

struct Time : public std::tm
{
	// inheriting from struct tm

	// Extending milliseconds
	int tm_msec;
}; 

struct DateTime
{
    char relative;
    Time dateTime; 
    char timezone;

    static const std::string defaultFormat;

    DateTime() : relative(0), timezone(0) {}

    /**
     * convert to string
     */
    std::string toString();

    /**
     * Convert to base time
     * Does not fully support the relative time and timezone yet
     * \return base time
     */
    base::Time toTime();
}; 




}} // namespace fipa::acl
#endif

