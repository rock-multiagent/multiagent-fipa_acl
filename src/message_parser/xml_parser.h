#ifndef FIPAACL_XML_PARSER_H
#define FIPAACL_XML_PARSER_H

#include "tinyxml.h"
#include <string>
#include <base/time.h>
#include <fipa_acl/message_generator/acl_envelope.h>
#include <fipa_acl/message_generator/acl_message.h>
#include <fipa_acl/message_generator/types.h>

namespace fipa {
namespace acl {

/**
 * A set of static methods to parse certain XML elemenets containing FIPA compliant data.
 * All methods throw an exception of the content is not FIPA compliant.
 */
class XMLParser
{
public:
    static const ReceivedObject parseReceivedObject(const TiXmlElement* receivedObjectElem);
    static const base::Time parseDate(const TiXmlElement* dateElem);
    static const base::Time strToDate(const std::string& dateStr);
    static const std::string parseURL(const TiXmlElement* urlElem);
    static const std::string parseName(const TiXmlElement* nameElem);
    static const AgentID parseAgentID(const TiXmlElement* aidElem);
    static const AgentIDList parseAgentIDSequence(const TiXmlElement* aidlElem);
    static const UserdefParam parseUserdefinedParameter(const TiXmlElement* paramElem);
    /**
     * For all message parameters FIPA allows to set the content directly or to point to
     * an URL with a href attribute. The content of either will be returned or NULL
     * if none is set.
     */
    static const char* extractContentOrAttribute(const TiXmlElement* elem, const char* identifier = "href");
};

} // end namespace acl
} // end namespace fipa

#endif // FIPAACL_XML_MESSAGE_PARSER_H
