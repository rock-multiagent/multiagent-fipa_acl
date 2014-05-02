#include "envelope_parser.h"
#include "bitefficient_envelope_parser.h"
#include "xml_envelope_parser.h"

#include <boost/assign/list_of.hpp>

namespace fipa {
namespace acl {
    
std::map<representation::Type, EnvelopeParserImplementationPtr > EnvelopeParser::msParsers = boost::assign::map_list_of
        (representation::BITEFFICIENT, boost::shared_ptr<EnvelopeParserImplementation>(new BitefficientEnvelopeParser()) )
        (representation::XML, boost::shared_ptr<EnvelopeParserImplementation>(new XMLEnvelopeParser()) );


bool EnvelopeParser::parseData(const std::string& storage, ACLEnvelope& envelope, representation::Type type)
{
    EnvelopeParserImplementationPtr messageParser = msParsers[type];
    if(messageParser)
    {
        return messageParser->parseData(storage, envelope);
    } else {
        std::string msg = "EnvelopeParser: there is no parser registered for " + representation::TypeTxt[type];
        throw std::runtime_error(msg);
    }
}

} // end namespace acl
} // end namespace fipa
