#ifndef FIPA_ACL_STRING_MESSAGE_FORMAT_H
#define FIPA_ACL_STRING_MESSAGE_FORMAT_H

#include <fipa_acl/message_generator/format/string_format.h>
namespace fipa {
namespace acl {

class StringMessageFormat : public MessageFormat
{
public:
    /**
     * Applies the string format to the message
     * \return the formatted message
     */
    std::string apply(const ACLMessage& msg) const;
};

} // end namespace acl
} // end namespace fipa

#endif //FIPA_ACL_STRING_MESSAGE_FORMAT_H
