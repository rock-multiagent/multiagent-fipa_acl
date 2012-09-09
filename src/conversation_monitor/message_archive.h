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
     */
    const ACLMessage& getInitiatingMessage() const;

    /**
     * Add a message to the archive
     */
    void addMessage(const ACLMessage& msg);
};

} // end namespace acl
} // end namespace fipa
#endif // FIPAACL_CONVERSATION_MONITOR_MESSAGE_ARCHIVE_H
