/**
 *
 * \file bitefficient_message_parser.h
 * \author Mircea Cretu Stancu
 * \author Thomas Roehr, thomas.roehr@dfki.de
 * \brief rebuilds an ACLMessage from a parsed bit-efficient encoded received message
 */
#ifndef FIPAACL_BITEFFICIENT_MESSAGE_PARSER_H
#define FIPAACL_BITEFFICIENT_MESSAGE_PARSER_H

#include <vector>
#include <string>

#include <fipa_acl/message_generator/acl_message.h>
#include <fipa_acl/message_generator/userdef_param.h>
#include <fipa_acl/message_generator/agent_id.h>

#include <fipa_acl/message_parser/types.h>

#include <fipa_acl/message_parser/message_parser.h>

/**
* \brief Foundation of Physical Intelligent Agents
*/
namespace fipa { 

/** 
* \brief Agent Communication Language
*/
namespace acl {

/**
* \class BitefficientMessageParser
* \brief Allows to parse a bytestream that conforms to the bitefficient representation of FIPA messages to a ACLMessage
* \details Given a bytestream that conforms to the FIPA Standard for the bitefficient representation of messages, this class can be used
* to parse this stream into a ACLMessage. The parsing makes extensive use of the boost::spirit library, a library whose contribution
* we want to acknowledge here. The Spirit library has been used to define the grammar and the underlying parser, i.e. the core of this library. 
* 
* Internals: Since the parser works with a number of customized data structures to facilitate parsing, a number of function have been implemented
* in order to perform the conversion to the datastructure required for a ACLMessage. Yet (by August 2010), we haven't done any performance evaluation of the library. 
*  
*/
class BitefficientMessageParser : public MessageParserImplementation
{
	public: 
		/**
		  \brief parses a correctly encoded message according to grammar_bitefficient.h and creates a Message object for internal use
		* \param storage Array of bytes that represent the bitefficient FIPA encoded message
		* \param msg The message extracted from the data
                * \param representation the representation to decode the incoming message
		* \return The decoded ACLMessage object
		*/	
		bool parseData(const std::string& storage, ACLMessage &msg);

		/**
		    \brief creates an unpopulated message object and passes it around to have its fields populated, along with the parsed message where the data is extracted from
		    \param parsedMsg The (intermediate) message from the spirit parser
		    \param msg The (final) ACLMessage to be filled with the parsed data
		    \return the decoded ACLMessage object
		*/
		bool buildMessage(const Message& parsedMsg, ACLMessage &msg);

	private:
                /**
                 * Fill the content field from message parameter object
                 */
                void buildContent(const message::Parameter& param,ACLMessage &msg);
		
 		/**
                 *  Since the parser relies heavily on customized data structures,
                 *  we require to apply conversion functions
                 */
		void buildParameters(const std::vector<message::Parameter>& parsedParams,ACLMessage &msg);

                /**
                 * Build predefined message parameters
                 * \throw std::runtime_error if message parameter is unknown
                 */
		void buildPredefMessageParameters(const message::Parameter& param, ACLMessage &msg);

                /**
                 * Build sender from message parameter
                 */
		void buildSender(const message::Parameter& param, ACLMessage &msg);

                /**
                 * Build agent id from agent identifier
                 */
		AgentID buildAgentID(const AgentIdentifier& agent);

                /**
                 * Build resolvers from agent identifier
                 */
		void buildResolvers(const AgentIdentifier& agentIdentifier, AgentID& outAgentID);

                /**
                 * Build agent id from agent identifier
                 */
		void buildAgentParameters(const AgentIdentifier& agentIdentifier, AgentID& outAgentID);

                /**
                 * Build user defined parameter from parser data type
                 */
		UserdefParam buildUserdefParameter(const UserDefinedParameter& param);

                 /**
                  * Build userdefined parameter from message parameter
                  */
		UserdefParam buildUserdefParameter(const message::Parameter& param);

                /**
                 * Build receiver from message parameter
                 */
		void buildReceiver(const message::Parameter& param, ACLMessage &msg);

                /**
                 * Build reply by from message parameter
                 */
		void buildReplyBy(const message::Parameter& param, ACLMessage &msg);

                /**
                 * Build reply to from message parameter
                 */
		void buildReplyTo(const message::Parameter& param, ACLMessage &msg);
};


}
}

#endif // FIPAACL_BITEFFICIENT_MESSAGE_PARSER_H
