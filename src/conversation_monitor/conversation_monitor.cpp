#include "conversation_monitor.h"
#include <base/Logging.hpp>

namespace fipa {
namespace acl {

ConversationMonitor::ConversationMonitor(const AgentID& self, const std::string& protocolDirectory)
    : mSelf(self)
{
    LOG_DEBUG("Creating conversation monitor for agent: '%s'", self.getName().c_str());
    if(!protocolDirectory.empty())
    {
        LOG_DEBUG("Setting protocol resource directory: '%s'",protocolDirectory.c_str());
        fipa::acl::StateMachineFactory::setProtocolResourceDir( protocolDirectory );
    }
}

ConversationMonitor::~ConversationMonitor()
{}

ConversationPtr ConversationMonitor::updateConversation(const fipa::acl::ACLMessage& msg)
{
    boost::unique_lock<boost::mutex> lock(mMutex);
    std::map<std::string, ConversationPtr>::iterator it;
    std::string conversationId = msg.getConversationID();
    it = mActiveConversations.find(conversationId);
    // update if conversation already exists
    if(it != mActiveConversations.end())
    {
        bool conversationEnded = false;
        try {
            conversationEnded = it->second->hasEnded();
        } catch(const std::runtime_error& e)
        {}

        if(conversationEnded)
        {
            std::string errorMsg = "Trying to update already completed conversation: " + conversationId + " performative: '" + msg.getPerformative() + "' content: '" + msg.getContent() + "'";
            throw conversation::InvalidOperation(errorMsg);
        } else {
            LOG_INFO("Update existing conversation '%p' with conversation id '%s'", this, it->second->getConversationId().c_str());
            it->second->update(msg);
            return it->second;
        }
    }

    throw conversation::InvalidOperation("Can only update an existing conversation. Possibly the corresponding conversation did already end and this message does not conform to the respective protocol");
}

bool ConversationMonitor::removeConversation(const fipa::acl::ConversationID& conversationId)
{
    boost::unique_lock<boost::mutex> lock(mMutex);
    bool success = mActiveConversations.erase(conversationId);
    return success;
}

ConversationPtr ConversationMonitor::startConversation(const std::string& topic)
{
    boost::unique_lock<boost::mutex> lock(mMutex);
    // Start conversation with this agent as owner
    ConversationPtr conversation(new Conversation(mSelf.getName(), Conversation::generateConversationID(topic) ));
    mActiveConversations.insert(std::pair<std::string, ConversationPtr>(conversation->getConversationId(), conversation));
    return conversation;
}

ConversationPtr ConversationMonitor::getConversation(const fipa::acl::ConversationID& conversationId)
{
    // get conversation associated with command of uuid
    boost::unique_lock<boost::mutex> lock(mMutex);
    std::map<fipa::acl::ConversationID, ConversationPtr>::iterator it = mActiveConversations.find(conversationId);
    if(it != mActiveConversations.end())
        return it->second;

    return ConversationPtr();
}


ConversationPtr ConversationMonitor::getOrCreateConversation(const fipa::acl::ConversationID& conversationId)
{
    // get conversation associated with comman of uuid
    boost::unique_lock<boost::mutex> lock(mMutex);
    std::map<fipa::acl::ConversationID, ConversationPtr>::iterator it = mActiveConversations.find(conversationId);
    if(it != mActiveConversations.end())
        return it->second;

    // create if it does not exist
    ConversationPtr conversation(new Conversation(mSelf.getName(), conversationId));
    mActiveConversations.insert(std::pair<fipa::acl::ConversationID, ConversationPtr>(conversationId, conversation));
    LOG_INFO("Create new conversation '%p' with conversation id '%s'", this, conversationId.c_str());
    return conversation;
}

void ConversationMonitor::cleanup()
{
    boost::unique_lock<boost::mutex> lock(mMutex);

    std::map<fipa::acl::ConversationID, ConversationPtr>::iterator it = mActiveConversations.begin();
    std::vector<fipa::acl::ConversationID> endedConversations;
    std::vector<fipa::acl::ConversationID>::iterator eit;

    // If conversation has ended move from active conversation to ended conversation after detaching 
    // any existing observers
    for(; it != mActiveConversations.end(); ++it)
    {
        ConversationPtr conversation = it->second;
        assert(it->second);
        if(conversation->hasEnded())
        {
            conversation->detachObservers();
            endedConversations.push_back(it->first);
        }
    }
    
    for( eit = endedConversations.begin(); eit != endedConversations.end(); ++eit)
    {
        mActiveConversations.erase(*eit);
    }
}

std::vector<fipa::acl::ConversationID> ConversationMonitor::getActiveConversations()
{
    boost::unique_lock<boost::mutex> lock(mMutex);
    std::map<fipa::acl::ConversationID, ConversationPtr>::iterator it = mActiveConversations.begin();
    std::vector<fipa::acl::ConversationID> activeConversations;

    for(; it != mActiveConversations.end(); it++)
    {
        activeConversations.push_back(it->first);
    }

    return activeConversations;
}

} // end namespace acl
} // end namespace fipa
