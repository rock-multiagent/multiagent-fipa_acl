#ifndef __FIPAACL_MESSAGEPARSER_MSGPRINTER_H_
#define __FIPAACL_MESSAGEPARSER_MSGPRINTER_H_

#include <fipa_acl/message_parser/types.h>

namespace fipa { namespace acl {

/**
* \brief Allows printing of a newly constructed fipa::acl::Message
*/
class MessagePrinter 
{
public:
	MessagePrinter();
	~MessagePrinter();
	
	/**
         * Print the output of a fipa message to stdout
	 * \param msg
         */
	void print(const fipa::acl::Message& msg) const;
};

}} // namespace fipa::acl
#endif

