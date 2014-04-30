#ifndef FIPA_ACL_XML_FORMAT_H
#define FIPA_ACL_XML_FORMAT_H

#include <string>
#include <base/time.h>
#include <fipa_acl/message_generator/acl_envelope.h>
#include <fipa_acl/message_generator/acl_message.h>
#include <fipa_acl/message_generator/types.h>
#include "tinyxml.h"

namespace fipa {
namespace acl {

class XMLFormat
{
public:
    static TiXmlElement* getReceivedObject(const ReceivedObject& receivedObject);
    static TiXmlElement* getDate(const base::Time& date);
    static const std::string dateToStr(const base::Time& date);
    static TiXmlElement* getURL(const std::string& url, bool useHref);
    static TiXmlElement* getName(const std::string& name, bool useId);
    static TiXmlElement* getAgentID(const AgentID& aid, bool useNameId, bool useUrlHref);
    static std::vector< TiXmlElement* > getAgentIDSequence(const AgentIDList& aidl, bool useNameId, bool useUrlHref);
    static std::vector< TiXmlElement* > getUserdefinedParameters(const UserdefinedParameterList& params);
};

} // end namespace acl
} // end namespace fipa

#endif //FIPA_ACL_XML_FORMAT_H
