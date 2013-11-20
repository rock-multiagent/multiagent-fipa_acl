#include "bitefficient_message_parser.h"
#include "string_message_parser.h"


namespace fipa { 
namespace acl {

MessageParser::MessageParser()
{
    mParsers[representation::BITEFFICIENT] = new BitefficientMessageParser();
    mParsers[representation::STRING_REP] = new StringMessageParser();
}

MessageParser::~MessageParser()
{
    std::map<representation::Type, MessageParserImplementation*>::iterator it = mParsers.begin();
    for(; it != mParsers.end(); ++it)
    {
        delete it->second;
        it->second = NULL;
    }
}

bool MessageParser::parseData(const std::string& storage, ACLMessage &msg, fipa::acl::representation::Type representation)
{
    MessageParserImplementation* messageParser = mParsers[representation];
    if(messageParser)
    {
        return messageParser->parseData(storage, msg);
    } else {
        std::string msg = "MessageParser: there is no parser registered for " + representation::TypeTxt[representation];
        throw std::runtime_error(msg);
    }
}

} // end namespace acl
} // end namespace fipa
