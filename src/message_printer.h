#ifndef __FIPAACL_MESSAGEPARSER_MSGPRINTER_H_
#define __FIPAACL_MESSAGEPARSER_MSGPRINTER_H_

#include "types.h"

namespace fipa { namespace acl {

class MessagePrinter 
{
public:
	MessagePrinter();
	~MessagePrinter();

	void print(const fipa::acl::Message& msg);
};

}} // namespace fipa::acl
#endif

