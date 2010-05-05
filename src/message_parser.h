/**
 *
 * \file message_parser.h
 * \author Mircea Cretu Stancu
 * \brief rebuilds an ACLMessage from a parsed bit-efficient encoded received message
 * 
 * \version 1.0
 */
#ifndef RIMRES_FIPAACL_MESSAGE_PARSER_H_
#define RIMRES_FIPAACL_MESSAGE_PARSER_H_

#include "message-generator/ACLMessage.h"
#include "message-generator/UserdefParam.h"
#include "message-generator/AgentAID.h"
#include "grammar_bitefficient.h"

#include <vector>
#include <string>

namespace fipa { 
namespace acl {

class MessageParser
{
	
	public: 
		MessageParser();
		~MessageParser();
	
		/**
		* \param storage Array of bytes that represent the bitefficient FIPA message
		* \return A message object for internal use
		*/	
		ACLMessage* parseData(const std::string storage);
		
		ACLMessage* buildMessage(Message parsedMsg);
		void buildParameters(std::vector<MessageParameter> parsedParams,ACLMessage* msg);
		int buildPredefMessageParameters(MessageParameter param,ACLMessage* msg);
		void buildSender(MessageParameter param,ACLMessage* msg);
		AgentAID* buildAgentAID(AgentID agent);
		void buildResolvers(AgentAID *workAg, AgentID agent);
		void buildAgentParameters(AgentAID* workAg, AgentID agent);
		UserdefParam* buildUserdefParameter(UserDefinedParameter param);
		UserdefParam* buildUserdefParameter(MessageParameter param);
		void buildReceiver(MessageParameter param, ACLMessage *msg);
		void buildReplyWith(MessageParameter param, ACLMessage *msg);
		void buildReplyBy1(MessageParameter param, ACLMessage *msg);
		void buildInReplyTo(MessageParameter param, ACLMessage *msg);
		void buildReplyTo(MessageParameter param, ACLMessage *msg);
		void buildLanguage(MessageParameter param, ACLMessage *msg);
		void buildEncoding(MessageParameter param, ACLMessage *msg);
		void buildOntology(MessageParameter param, ACLMessage *msg);
		void buildProtocol(MessageParameter param, ACLMessage *msg);
		void buildConversationID(MessageParameter param, ACLMessage *msg);
		void buildContent(MessageParameter param,ACLMessage* msg);

};


}
}

#endif // RIMRES_FIPAACL_MESSAGE_PARSER_H_
