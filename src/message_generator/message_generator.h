#ifndef FIPA_ACL_MESSAGE_GENERATOR_H
#define FIPA_ACL_MESSAGE_GENERATOR_H

#include <fipa_acl/message_generator/types.h>
#include <fipa_acl/message_generator/acl_message.h>
#include <fipa_acl/message_generator/message_format.h>
#include <fipa_acl/message_generator/agent_id.h>
#include <boost/shared_ptr.hpp>

namespace fipa {
namespace acl {

namespace message_format
{
    enum Type { NONE, BITEFFICIENT };
}

typedef boost::shared_ptr<MessageFormat> MessageFormatPtr;

class MessageGenerator
{
    static std::map<message_format::Type, MessageFormatPtr> msFormats;
public:

    /**
     * Create a message of a certain message type
     * \return message object of the select format
     */
    static std::string create(const ACLMessage& msg, message_format::Type type);
};


} // end namespace acl
} // end namespace fipa

#endif // _MESSAGE_GENERATOR_H_
