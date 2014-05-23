#ifndef FIPA_ACL_XML_ENVELOPE_PARSER_H
#define FIPA_ACL_XML_ENVELOPE_PARSER_H

#include <string>
#include <fipa_acl/message_generator/acl_envelope.h>
#include <fipa_acl/message_generator/types.h>
#include <fipa_acl/message_parser/envelope_parser.h>
#include <tinyxml.h>

namespace fipa {
namespace acl {

class XMLEnvelopeParser : public EnvelopeParserImplementation
{
private:
    /**
     * Creates an ACLBaseEnvelope from the informations given in the params element.
     * Throws an exception, if invalid data is contained.
     */
    const ACLBaseEnvelope parseParameters(const TiXmlElement* paramsElem, int paramsIndex) const;

public: 
    bool parseData(const std::string& storage, ACLEnvelope& envelope);
};

} // end namespace acl
} // end namespace fipa

#endif // FIPA_ACL_XML_ENVELOPE_PARSER_H
