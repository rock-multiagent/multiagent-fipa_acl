#ifndef FIPA_ACL_MESSAGE_GENERATOR_H
#define FIPA_ACL_MESSAGE_GENERATOR_H

#include <fipa_acl/message_generator/types.h>
#include <fipa_acl/message_generator/acl_message.h>
#include <fipa_acl/message_generator/acl_message_output_parser.h>
#include <fipa_acl/message_generator/agent_id.h>

namespace fipa {
namespace acl {

namespace message_format
{
    enum Type { NONE, BITEFFICIENT };
}

class MessageGenerator
{
    const std::map<message_format::Type, MessageFormat> mFormats;
public:

    /**
     * Default constructor
     */
    MessageGenerator();

    /**
     * Create a message of a certain message type
     * \return message object of the select format
     */
    std::string create(const ACLMessage& msg, message_format::Type type) const;
};

} // end namespace acl
} // end namespace fipa

#endif // _MESSAGE_GENERATOR_H_
