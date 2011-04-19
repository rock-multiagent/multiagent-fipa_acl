#ifndef __FIPAACL_MESSAGEPARSER_DATETIME_H_
#define __FIPAACL_MESSAGEPARSER_DATETIME_H_

#include <stdio.h>
#include <string>
#include <ctime>

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

   DateTime() : relative(0), timezone(0) {}

   std::string toString()
   {
	char buffer[512];
	// %Z missing so far
	strftime(buffer,512, "%Y-%m-%dT%H:%M:%S", &dateTime);

	std::string formattedOutput;
	formattedOutput += relative;
	formattedOutput += std::string(buffer);
	formattedOutput += timezone;

	return formattedOutput;
   }

}; 




}} // namespace fipa::acl
#endif

