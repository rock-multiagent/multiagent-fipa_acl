#ifndef RIMRES_FIPAACL_MESSAGE_PARSER_H_
#define RIMRES_FIPAACL_MESSAGE_PARSER_H_

#include "message-generator/ACLMessage.h"
#include <stdint.h>
#include <vector>

namespace fipa { namespace acl {

class MessageParser
{
	
	public: 
		MessageParser();
		~MessageParser();
	
		/**
		* \param data Array of bytes that represent the bitefficient FIPA message
		* \return A message object for internal use
		*/	
		fipa::acl::ACLMessage parseData(const std::vector<uint8_t>& data);

};


}}

#endif // RIMRES_FIPAACL_MESSAGE_PARSER_H_
