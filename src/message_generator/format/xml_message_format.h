#ifndef FIPA_ACL_XML_MESSAGE_FORMAT_H
#define FIPA_ACL_XML_MESSAGE_FORMAT_H

#include <fipa_acl/message_generator/format/xml_format.h>
#include <fipa_acl/message_generator/message_format.h>
#include "tinyxml.h"

namespace fipa {
namespace acl {

class XMLMessageFormat : public MessageFormat
{
private:
    /**
     * Encode message parameters
     */
    std::vector< TiXmlElement* > getParameters(const ACLMessage& aclMsg) const;

public:
    /**
     * Applies the XML format to the message
     * \return the formatted message
     */
    std::string apply(const ACLMessage& aclMsg) const;
};

} // end namespace acl
} // end namespace fipa

#endif //FIPA_ACL_XML_MESSAGE_FORMAT_H
