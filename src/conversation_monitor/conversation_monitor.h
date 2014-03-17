#ifndef FIPA_ACL_CONVERSATION_MONITOR_H
#define FIPA_ACL_CONVERSATION_MONITOR_H

#include <map>
#include <vector>
#include <boost/thread.hpp>

#include <fipa_acl/conversation_monitor/conversation.h>
#include <fipa_acl/message_generator/acl_message.h>

namespace fipa {
namespace acl {

class Configuration;

/**
* The ConversationMonitor allows to follow conversation between multiple agents
* and eventually allows to verify the conversation flow against an interaction 
* protocol
*/
class ConversationMonitor
{

public: 
    /**
     * Construct the conversation monitor using the system configuration object
     * which for example holds the name of the agent
     * \param self AgendID of the current agent
     * \param protocolDirectory Directory of interaction protocols
     *
     */
    ConversationMonitor(const AgentID& self, const std::string& protocolDirectory = "");

    /**
     * Default deconstructor
     */    
    virtual ~ConversationMonitor();

    /**
    * When an internal message has been generated as a response or as an initiation of 
    * a conversation update the conversation monitor using this function
    * \param the outgoing message
    * \return pointer to the conversation
    * \throw InvalidOperation if conversation does not exist this message
    * \throw ProtocolError if conversation cannot consume the message due to an
    * invalid protocol flow
    */
    ConversationPtr updateConversation(const fipa::acl::ACLMessage& msg);

    /**
    * Start a conversation without a message - this is required for internal requests
    * refer to OutgoingMessageHandler::operator()()
    * The id will be automatically generated at construction time of Conversation and extended
    * by the topic if required
    * \param topic Topic to add to the automatically generated conversation id
    * \return Pointer to the created conversation
    */
    ConversationPtr startConversation(const std::string& topic);

    /**
    * Close a conversation with the provided conversationId
    */ 
    bool removeConversation(const fipa::acl::ConversationID& conversationId);

    /**
    * Get conversation associated with a certain conversation id
    * User has to verify that id of conversation is unqual to an empty string
    */
    ConversationPtr getConversation(const fipa::acl::ConversationID& conversationId);

    /**
    * Get conversation associated with a certain conversation id or create a new
    * one if it does not exist yet
    */
    ConversationPtr getOrCreateConversation(const fipa::acl::ConversationID& conversationId); 

    /**
    * Cleanup all conversations, that ended
    */
    void cleanup();
    
    /**
    * Get a list of conversation ids of all active conversations
    * \return List of conversation ids currently active conversations 
    */
    std::vector<fipa::acl::ConversationID> getActiveConversations();

private:
    AgentID mSelf;

    std::map<fipa::acl::ConversationID, ConversationPtr> mActiveConversations;

    boost::mutex mMutex;
};

} // end namespace acl
} // end namespace fipa
#endif // FIPA_ACL_CONVERSATION_MONITOR_H
