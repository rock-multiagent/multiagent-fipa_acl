#ifndef FIPAACL_MESSAGE_PARSER_H
#define FIPAACL_MESSAGE_PARSER_H

#include <map>
#include <fipa_acl/message_generator/acl_message.h>
#include <fipa_acl/message_generator/types.h>

namespace fipa { 
namespace acl {

class MessageParserImplementation
{
    public:
        virtual bool parseData(const std::string& storage, ACLMessage& msg) { throw std::runtime_error("Parser not implemented"); }
};

/**
* \class MessageParser
* \brief Allows to parse a bytestream that conforms to the bitefficient representation of FIPA messages to a ACLMessage
* \details Given a bytestream that conforms to the FIPA Standard for the bitefficient representation of messages, this class can be used
* to parse this stream into a ACLMessage. The parsing makes extensive use of the boost::spirit library, a library whose contribution
* we want to acknowledge here. The Spirit library has been used to define the grammar and the underlying parser, i.e. the core of this library. 
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
        * \param representation the representation to decode the incoming message
        * \return The decoded ACLMessage object
        */	
	bool parseData(const std::string& storage, ACLMessage &msg, fipa::acl::representation::Type representation = fipa::acl::representation::BITEFFICIENT);

    private:
        std::map<fipa::acl::representation::Type, MessageParserImplementation*> mParsers;
};


}
}

#endif // FIPAACL_MESSAGE_PARSER_H_
