#include "conversation.h"
#include <uuid/uuid.h>
#include <base/Logging.hpp>

namespace fipa {
namespace acl {

namespace conversation {

Event::Event()
    : msg()
    , type(UNKNOWN)
    , timestamp()
{}

Event::Event(const fipa::acl::ACLMessage& _msg, EventType _type)
    : msg(_msg)
    , type(_type)
    , timestamp(base::Time::now())
{}

} // end namespace conversation

fipa::acl::StateMachineFactory Conversation::msStateMachineFactory;

ConversationObserver::ConversationObserver()
{}

bool ConversationObserver::hasEvents() const
{
    boost::unique_lock<boost::mutex> lock(mEventsMutex);
    return !mEvents.empty();
}

void ConversationObserver::update(const conversation::Event& event)
{
    boost::unique_lock<boost::mutex> lock(mEventsMutex);
    mEvents.push_back(event);
    mCondition.notify_all();
}

conversation::Event ConversationObserver::waitForNextEvent()
{
    boost::mutex mutex;
    {
        boost::unique_lock<boost::mutex>  lock(mutex);
        while(!hasEvents())
        {
            mCondition.wait(lock);
        }
    }

    conversation::Event event;
    assert(getNextEvent(event));

    return event;
}

bool ConversationObserver::getNextEvent(conversation::Event& event)
{
    boost::unique_lock<boost::mutex> lock(mEventsMutex);
    if(mEvents.empty())
    {
        return false;
    }

    event = mEvents.front();
    mEvents.erase(mEvents.begin());

    LOG_INFO("Retrieve event: type: '%s', msg content: '%s'", conversation::EventTypeTxt[event.type].c_str(), event.msg.getContent().c_str());
    return true;
}

std::vector<fipa::acl::ConversationID> ConversationObserver::getConversationIdsOfObservables() const
{
    std::vector<fipa::acl::ConversationID> conversationIds;

    boost::unique_lock<boost::mutex> lock(mObservablesMutex);
    std::vector<ConversationObservablePtr>::const_iterator it = mObservables.begin();
    for(; it != mObservables.end(); ++it)
    {
        conversationIds.push_back((*it)->getConversationId());
    }

    return conversationIds;
}

void ConversationObserver::registerObservable(const ConversationObservablePtr& observable)
{
    boost::unique_lock<boost::mutex> lock(mObservablesMutex);

    std::vector<ConversationObservablePtr>::const_iterator it = std::find(mObservables.begin(), mObservables.end(), observable);
    if(it == mObservables.end())
    {
        mObservables.push_back(observable);
    }
}

Conversation::Conversation(const std::string& owner, const fipa::acl::ConversationID& conversationId)
   : ConversationObservable(conversationId)
   , mOwner(owner)
{
    if(mConversationId.empty())
    {
        mConversationId = generateConversationID();
    }
    LOG_DEBUG("Conversation created with id: %s\n", conversationId.c_str());
}

Conversation::Conversation(const std::string& owner, const fipa::acl::ACLMessage& initiator)
   : ConversationObservable()
   , mOwner(owner)
{
    update(initiator);
    LOG_DEBUG("Conversation created with id: %s\n", initiator.getConversationID().c_str());
    assert(!mConversationId.empty());
}

Conversation::~Conversation()
{
}

std::string Conversation::getProtocol() const
{
    return mProtocol;
}

std::string Conversation::getContentLanguage() const
{
    return mContentLanguage;
}

void Conversation::update(const fipa::acl::ACLMessage& msg)
{
    boost::unique_lock<boost::mutex> lock(mMutex);

    LOG_INFO("Update conversation: id '%s'", msg.getConversationID().c_str());
    LOG_INFO("Update message: performative: '%s' content: '%s'", msg.getPerformative().c_str(), msg.getContent().c_str());


    bool newConversation = false;
    try {
        // If protocol is unset we don't have a valid message
        // Thus at this point the conversation is not yet initialized
        if(mProtocol.empty())
        {
            std::string protocol = msg.getProtocol();
            try {
                if(!protocol.empty())
                {
                    mStateMachine = msStateMachineFactory.getStateMachine(protocol);
                    mStateMachine.setSelf( fipa::acl::AgentID(mOwner) );

                    mProtocol = protocol;
                    mContentLanguage = msg.getLanguage();
                    mConversationId = msg.getConversationID();

                } else {
                    LOG_ERROR("Protocol not set");
                    throw std::runtime_error("Protocol not set");
                }
            } catch(const std::runtime_error& e)
            {
                LOG_FATAL("Conversation could not retrieve statemachine for protocol '%s'. Check if protocol specification was loaded -- '%s'", protocol.c_str(), e.what());
                throw;
            }

            newConversation = true;

        } else if( mProtocol != msg.getProtocol())
        {
            // This probably means, it's a subProtocol message
            updateSubProtocol(msg);
            return;
        } else if( msg.getProtocol().empty())
        {
            LOG_WARN("Conversation: received message has no protocol being set. Current conversation using '%s'", mProtocol.c_str());
        } else if( msg.getLanguage().empty())
        {
            LOG_WARN("Conversation: received message has not language being set. Current conversation using '%s'", mContentLanguage.c_str());
        } else if(mContentLanguage != msg.getLanguage())
        {
            LOG_ERROR("Conversation: message with different content language being inserted: current '%s' - to be inserted '%s'", mContentLanguage.c_str(), msg.getLanguage().c_str());
            throw std::runtime_error("Conversation: conversation with multiple content language are not supported");
        }

#ifdef NDEBUG
        std::string receiver;
        fipa::acl::AgentIDList receivers = msg.getAllReceivers();
        if(!receivers.empty())
        {
            receiver = receivers[0].getName();
        }


        LOG_INFO("StateMachine consume message '%s' for owner '%s' with performative '%s' sender '%s' receiver '%s' ", msg.getConversationID().c_str(), mStateMachine.getOwner().getName().c_str(), msg.getPerformative().c_str(), msg.getSender().getName().c_str(),
                receiver.c_str());
#endif
        // update the message state machine
        try {
            mStateMachine.consumeMessage(msg);
        } catch(const std::runtime_error& e)
        {
            std::string errorMsg = "Conversation: unexpected message with performative '" + msg.getPerformative() + "' for the protocol '" + msg.getProtocol() + "' ";
            errorMsg += " current state: '" + mStateMachine.getCurrentStateId() + "'";
            errorMsg += " -- " + std::string(e.what());
            throw conversation::ProtocolException(errorMsg);
        }

        notifyAll(msg, newConversation);

    } catch(...)
    {
        // Notify only to handle internally
        throw;
    }
}

void Conversation::notifyAll(const fipa::acl::ACLMessage& msg, bool newConversation)
{
    mMessages.push_back(msg);

    if(mStateMachine.inFailureState())
    {
        notify(msg, conversation::FAILURE);
    } else if(hasEnded()) {
        // FIXME The end is being notified, even though there can be additional messages
        // if the final state has transitions.
        notify(msg, conversation::END_OF_CONVERSATION);
    } else if(newConversation) {
        notify(msg, conversation::START_OF_CONVERSATION);
    } else {
        notify(msg, conversation::INTERMEDIATE_UPDATE);
    }
}

void Conversation::updateSubProtocol(const fipa::acl::ACLMessage& msg)
{
    // Loop through all not-ended sub state machines
    std::vector<StateMachine>::iterator it0;
    for(it0 = mSubStateMachines.begin(); it0 != mSubStateMachines.end(); it0++)
    {
        if(it0->inFinalState() || it0->inFailureState())
        {
            // StateMachine ended already
            continue;
        }
        // Test if this StateMachine is correct by maintaining a copy of it
        StateMachine copy = *it0;
        
        // Test the update
        try {
            it0->consumeMessage(msg);
            // It worked
            notifyAll(msg, false);
            return;
        } catch(const std::runtime_error& e)
        {
            // The state machine was obviously not correct, we play back the copy
            *it0 = copy;
        }
    }
    
    // We must be in a state that allows subProtocols
    std::string protocol = msg.getProtocol();
    const State& currentState = mStateMachine.getCurrentState();
    const std::vector<EmbeddedStateMachine>& embeddedStateMachines =  currentState.getEmbeddedStatemachines();
    // Search for an embedded state machine with the same protocol
    const EmbeddedStateMachine* esmP;
    std::vector<EmbeddedStateMachine>::const_iterator it;
    for(it = embeddedStateMachines.begin(); it != embeddedStateMachines.end(); it++)
    {
        // Protocol and sender must match
        // TODO Regex matching instead if equality
        if(it->name == protocol)
        {           
            // Check that the sender role is correct
            // This throws if the mapping does not exist
            const AgentIDList l = mStateMachine.getRoleMapping().getMapping().at(it->fromRole);
            AgentIDList::const_iterator lit = std::find(l.begin(), l.end(), msg.getSender());
            if(lit == l.end())
            {
                // Sender does not match
                continue;
            }
            
            // This is the right ESM
            esmP = &(*it);
            break;
        }
    }
    
    if(!esmP)
    {
        // No fitting running or new embedded state machine found
        LOG_ERROR("Conversation: message with different protocol being inserted: current '%s' - to be inserted '%s'", mProtocol.c_str(), msg.getProtocol().c_str());
        throw conversation::ProtocolException("Conversation: message with wrong protocol being inserted");
    }
    
    // Construct a new state machine with mapped sender role
    if(!protocol.empty())
    {
        StateMachine subStateMachine = msStateMachineFactory.getStateMachine(protocol);
        subStateMachine.setSelf(msg.getSender());
        // custom parameter for the message and to identify subStateMachine in the future
        std::string subSMIdentifier = generateConversationID(currentState.getId());
        
        mSubStateMachines.push_back(subStateMachine);
        
        // update the message state machine
        try {
            subStateMachine.consumeMessage(msg);
        } catch(const std::runtime_error& e)
        {
            // Also constructing and using a new one did not work for that message.
            std::string errorMsg = "Conversation: unexpected message with performative '" + msg.getPerformative() + "' for the protocol '" + msg.getProtocol() + "' ";
            errorMsg += " current state: '" + mStateMachine.getCurrentStateId() + "'";
            errorMsg += " -- " + std::string(e.what());
            throw conversation::ProtocolException(errorMsg);
        }
    } else {
        LOG_ERROR("Protocol not set");
        throw std::runtime_error("Protocol not set");
    }
    
    notifyAll(msg, false);
}

fipa::acl::ACLMessage Conversation::getLastMessage() const
{
     boost::unique_lock<boost::mutex> lock(mMutex);
     return mMessages.back();
}

bool Conversation::hasEnded() const
{
    if(!mStateMachine.inFinalState())
    {
        LOG_DEBUG("Conversation did not end");
        return false;
        
    }
    // Check all subprotocols
    std::vector<StateMachine>::const_iterator it0;
    for(it0 = mSubStateMachines.begin(); it0 != mSubStateMachines.end(); it0++)
    {
        if(!it0->inFinalState() && !it0->inFailureState())
        {
            LOG_DEBUG("Conversation did not end (subconversation still running)");
            return false;
        }
    }      
    
    LOG_DEBUG("Conversation ended");
    return true;
}

bool Conversation::hasMessages() const
{
    boost::unique_lock<boost::mutex> lock(mMutex);
    return !mMessages.empty();
}

std::string Conversation::getOwner() const
{
    return mOwner;
}

fipa::acl::ConversationID Conversation::generateConversationID(const std::string& topic)
{
    uuid_t uuid;
    uuid_generate(uuid);

    char conversationId[37];
    uuid_unparse(uuid, conversationId);
    return std::string(conversationId) + "--" + topic + "--" + base::Time::now().toString();
}

ConversationObservable::ConversationObservable()
    : mStatus(conversation::UNINITIALIZED)
{}

ConversationObservable::ConversationObservable(const fipa::acl::ConversationID& conversationId)
    : mConversationId(conversationId)
    , mStatus(conversation::UNINITIALIZED)
{}

fipa::acl::ConversationID ConversationObservable::getConversationId() const
{
    return mConversationId;
}

bool ConversationObservable::hasObservers() const
{
    boost::unique_lock<boost::mutex> lock(mObserverMutex);
    return !mObservers.empty();
}

void ConversationObservable::addObserver(const ConversationObserverPtr& observer)
{
    boost::unique_lock<boost::mutex> lock(mObserverMutex);
    ConversationObserverList::const_iterator it = std::find(mObservers.begin(), mObservers.end(), observer);
    if(it == mObservers.end())
    {
        mObservers.push_back(observer);
        observer->registerObservable(ConversationObservablePtr(this, NullDeleter()));
    }
}

void ConversationObservable::removeObserver(const ConversationObserverPtr& observer)
{
    boost::unique_lock<boost::mutex> lock(mObserverMutex);
    ConversationObserverList::iterator it = std::find(mObservers.begin(), mObservers.end(), observer);
    if(it != mObservers.end())
    {
        mObservers.erase(it);
    }
}

void ConversationObservable::notify(const fipa::acl::ACLMessage& msg, conversation::EventType eventType)
{
    boost::unique_lock<boost::mutex> lock(mObserverMutex);
    LOG_INFO("notify: message event: '%s', message content: '%s'", conversation::EventTypeTxt[eventType].c_str(), msg.getContent().c_str());
    ConversationObserverList::iterator it = mObservers.begin();
    for(; it != mObservers.end(); ++it)
    {
        (*it)->update( conversation::Event(msg, eventType) );
    }

    switch(eventType)
    {
        case conversation::START_OF_CONVERSATION:
            assert(mStatus == conversation::UNINITIALIZED);
            mStatus = conversation::RUNNING;
            break;
        case conversation::END_OF_CONVERSATION:
            mStatus = conversation::ENDED;
            break;
        case conversation::INTERMEDIATE_UPDATE:
            mStatus = conversation::RUNNING;
            break;
        case conversation::FAILURE:
            mStatus = conversation::FAILED;
            break;
        default:
            break;
    }

}

conversation::Status ConversationObservable::getStatus() const
{
    return mStatus;
}

void ConversationObservable::detachObservers()
{
    mObservers.clear();
}

ConversationObserverList ConversationObservable::getObservers() const
{
    boost::unique_lock<boost::mutex> lock(mObserverMutex);
    return mObservers;
}

} // end namespace acl
} // end namespace fipa
