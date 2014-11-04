#ifndef FIPA_ACL_CONVERSATION_MONITOR_H
#define FIPA_ACL_CONVERSATION_MONITOR_H

#include <map>
#include <vector>
#include <boost/thread/mutex.hpp>

#include <fipa_acl/conversation_monitor/conversation.h>
#include <fipa_acl/message_generator/acl_message.h>

namespace fipa {
namespace acl {

class Configuration;

/**
 * \class ConversationMonitor
 * 
 * \brief This library provides a statemachine
 * to monitor a message flow based on performatives. The conversation monitor is capable of monitoring
 * the conversation based on the protocol specified in the messages. There are scxml documents specifying
 * how these protocols are defined. Protocols may contain subprotocols, which is partially supported.
 * 
 * The following sections provide an overview how to use the monitor and which constraints it has.
 * 
 * \section Examples
 * To verify a conversation, one could do the following:
\verbatim
ConversationMonitor monitor (selfAgentId);
ConversationPtr convPtr = monitor.startConversation("myTopic");
ACLMessage myMsg(ACLMessage::INFORM);
// Continue creating message, e.g. using inform protocol
myMsg.setProtocol("inform");
// ...
convPtr->update(myMsg);
if(convPtr->hasEnded())
{
    // ...
}
\endverbatim

If the conversation's protocol has subprotocols, at the point before starting a subProtocol, this becomes necessary:
\verbatim
// ...
convPtr->setNumberOfSubConversations(n);
// where n is the number of subconversations the agent plans to start.
\endverbatim

If one wishes to be notified of changes to a conversation, one can use the following after obtaining a ConversationPtr:
\verbatim
ConversationObserverPtr observer(new ConversationObserver());
convPtr->addObserver(observer);
// Method A:
conversation::Event event = observer->waitForNextEvent();
// ...

// Method B:
conversation::Event event;
if(observer->hasEvents() && observer->getNextEvent(event))
{
    // ...
}
\endverbatim

\section Constraints

At the moment, to handle subprotocols correctly a few constraints apply. If the agents to not obey these rules, the monitoring will fail.
- There can be only one substatemachine per state. More than one could work, but could also yield unexpected behaviour.
- For proxied protocols (like "brokering") that send the responses back there must be exactly _one_ forwarded reply; unless the proxied protocol is "inform", in which case there must be _no_ forwarded reply.
- Protocols where responses are sent directly to the initiator (not via a broker), like "recruiting", are not supported.
- There is no way to check if the started subprotocol was indeed the one specified in the proxy/propagate message.
- A propagating protocol cannot be terminated with a failure/cancel/not-understood.
- "setNumberOfSubConversations" has to be used for all sub-conversations.
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
