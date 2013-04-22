#ifndef FIPA_ACL_MESSAGE_GENERATOR_H
#define FIPA_ACL_MESSAGE_GENERATOR_H

#include <fipa_acl/message_generator/types.h>
#include <fipa_acl/message_generator/acl_message.h>
#include <fipa_acl/message_generator/message_format.h>
#include <fipa_acl/message_generator/agent_id.h>
#include <boost/shared_ptr.hpp>

namespace fipa {
namespace acl {

typedef boost::shared_ptr<MessageFormat> MessageFormatPtr;

class MessageGenerator
{
    static std::map<representation::Type, MessageFormatPtr> msFormats;
public:

    /**
     * Create a message of a certain acl representation (format), e.g. bitefficient, string, xml
     * \return message object using the given acl representation (format)
     */
    static std::string create(const ACLMessage& msg, const representation::Type& acl_representation);
};


} // end namespace acl
} // end namespace fipa

#endif // _MESSAGE_GENERATOR_H_
