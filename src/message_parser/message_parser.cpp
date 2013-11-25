#include "bitefficient_message_parser.h"
#include "string_message_parser.h"

#include <boost/assign/list_of.hpp>


namespace fipa { 
namespace acl {

std::map<representation::Type, MessageParserImplementationPtr > MessageParser::msParsers = boost::assign::map_list_of
            (representation::BITEFFICIENT, boost::shared_ptr<MessageParserImplementation>(new BitefficientMessageParser()) )
            (representation::STRING_REP, boost::shared_ptr<MessageParserImplementation>(new StringMessageParser()) );


bool MessageParser::parseData(const std::string& storage, ACLMessage &msg, fipa::acl::representation::Type representation)
{
    MessageParserImplementationPtr messageParser = msParsers[representation];
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
