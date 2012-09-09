/**
 *
 * \file message_parser.h
 * \author Mircea Cretu Stancu
 * \author Thomas Roehr, thomas.roehr@dfki.de
 * \brief rebuilds an ACLMessage from a parsed bit-efficient encoded received message
 * 
 * \version 1.0
 */
#ifndef FIPAACL_MESSAGE_PARSER_H_
#define FIPAACL_MESSAGE_PARSER_H_

#include <vector>
#include <string>

#include <fipa_acl/message_generator/acl_message.h>
#include <fipa_acl/message_generator/userdef_param.h>
#include <fipa_acl/message_generator/agent_id.h>

#include <fipa_acl/message_parser/types.h>

/**
* \brief Foundation of Physical Intelligent Agents
*/
namespace fipa { 

/** 
* \brief Agent Communication Language
*/
namespace acl {

/**
* \class MessageParser
* \brief Allows to parse a bytestream that conforms to the bitefficient representation of FIPA messages to a ACLMessage
* \details Given a bytestream that conforms to the FIPA Standard for the bitefficient representation of messages, this class can be used
* to parse this stream into a ACLMessage. The parsing makes extensive use of the boost::spirit library, a library whose contribution
* we want to acknowledge here. The Spirit library has been used to define the grammar and the underlying parser, i.e. the core of this library. 
* 
* Internals: Since the parser works with a number of customized data structures to facilitate parsing, a number of function have been implemented
* in order to perform the conversion to the datastructure required for a ACLMessage. Yet (by August 2010), we haven't done any performance evaluation of the library. 
*  
*/
class MessageParser
{
	
	public: 
		MessageParser();
		~MessageParser();
	
		/**
		  \brief parses a correctly encoded message according to grammar_bitefficient.h and creates a Message object for internal use
		* \param storage Array of bytes that represent the bitefficient FIPA encoded message
		* \param msg The message extracted from the data
		* \return The decoded ACLMessage object
		*/	
		bool parseData(const std::string storage, ACLMessage &msg);

		/**
		    \brief creates an unpopulated message object and passes it around to have its fields populated, along with the parsed message where the data is extracted from
		    \param parsedMsg The (intermediate) message from the spirit parser
		    \param msg The (final) ACLMessage to be filled with the parsed data
		    \return the decoded ACLMessage object
		*/
		bool buildMessage(Message parsedMsg, ACLMessage &msg);
		
	private:
 		/* Since the parser relies heavily on customized data structures, we require to apply conversion functions*/
		void buildParameters(std::vector<MessageParameter> parsedParams,ACLMessage &msg);

		int buildPredefMessageParameters(MessageParameter param,ACLMessage &msg);

		void buildSender(MessageParameter param, ACLMessage &msg);

		AgentID buildAgentID(AgentIdentifier agent);

		void buildResolvers(AgentID &workAg, AgentIdentifier agent);

		void buildAgentParameters(AgentID &workAg, AgentIdentifier agent);

		UserdefParam buildUserdefParameter(UserDefinedParameter param);

		UserdefParam buildUserdefParameter(MessageParameter param);

		void buildReceiver(MessageParameter param, ACLMessage &msg);

		void buildReplyWith(MessageParameter param, ACLMessage &msg);

		void buildReplyBy(MessageParameter param, ACLMessage &msg);

		void buildInReplyTo(MessageParameter param, ACLMessage &msg);

		void buildReplyTo(MessageParameter param, ACLMessage &msg);

		void buildLanguage(MessageParameter param, ACLMessage &msg);

		void buildEncoding(MessageParameter param, ACLMessage &msg);

		void buildOntology(MessageParameter param, ACLMessage &msg);

		void buildProtocol(MessageParameter param, ACLMessage &msg);

		void buildConversationID(MessageParameter param, ACLMessage &msg);

		void buildContent(MessageParameter param,ACLMessage &msg);



};


}
}

#endif // FIPAACL_MESSAGE_PARSER_H_
