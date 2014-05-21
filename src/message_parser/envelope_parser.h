#ifndef FIPA_ACL_ENVELOPE_PARSER_H
#define FIPA_ACL_ENVELOPE_PARSER_H

#include <fipa_acl/message_generator/acl_envelope.h>
#include <fipa_acl/message_generator/types.h>
#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

namespace fipa {
namespace acl {
    
class EnvelopeParserImplementation
{
    public:
        virtual bool parseData(const std::string& storage, ACLEnvelope& envelope) { throw std::runtime_error("Parser not implemented"); }
};

typedef boost::shared_ptr<EnvelopeParserImplementation> EnvelopeParserImplementationPtr;

class EnvelopeParser
{

public: 
    static bool parseData(const std::string& storage, ACLEnvelope& envelope, representation::Type type  = fipa::acl::representation::BITEFFICIENT);

private:
    static std::map<fipa::acl::representation::Type, fipa::acl::EnvelopeParserImplementationPtr> msParsers;

};

} // end namespace acl
} // end namespace fipa

#endif // FIPA_ACL_ENVELOPE_PARSER_H
