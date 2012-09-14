#ifndef FIPAACL_CONVERSATION_MONITOR_MESSAGE_ARCHIVE_H
#define FIPAACL_CONVERSATION_MONITOR_MESSAGE_ARCHIVE_H

#include <vector>
#include <fipa_acl/message_generator/acl_message.h>

namespace fipa {
namespace acl {

class MessageArchive
{
    // Messages
    std::vector<ACLMessage> mMessages;

public:

    /**
     * Get the last message that corresponds to the initiator
     * \throw std::runtime_error if no message is available
     */
    const ACLMessage& getInitiatingMessage() const;

    /**
     * Add a message to the archive
     */
    void addMessage(const ACLMessage& msg);

    /**
     * Test whether archive contains messages
     */
    bool hasMessages() const { return !mMessages.empty(); }
};

} // end namespace acl
} // end namespace fipa
#endif // FIPAACL_CONVERSATION_MONITOR_MESSAGE_ARCHIVE_H
