#include "conversation.h"
#include <uuid/uuid.h>
#include <base/Logging.hpp>
#include <boost/regex.hpp>

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
   , mNumberOfSubConversations(0)
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
   , mNumberOfSubConversations(0)
{
    update(initiator);
    LOG_DEBUG("Conversation created with id: %s\n", initiator.getConversationID().c_str());
    assert(!mConversationId.empty());
}

Conversation::Conversation(const Conversation& other)
    : ConversationObservable(other)
    , mOwner(other.mOwner)
    , mProtocol(other.mProtocol)
    , mContentLanguage(other.mContentLanguage)
    , mNumberOfSubConversations(other.mNumberOfSubConversations)
    , mMessages(other.mMessages)
    , mStateMachine(other.mStateMachine)
{
}

Conversation::~Conversation()
{
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
            try {
                mStateMachine.consumeSubStateMachineMessage(msg, msStateMachineFactory.getStateMachine(msg.getProtocol()), mNumberOfSubConversations);
            } catch(const std::runtime_error& e)
            {
                std::string errorMsg = "Conversation: unexpected message with performative '" + msg.getPerformative() + "' for the protocol '" + msg.getProtocol() + "' ";
                errorMsg += " current state: '" + mStateMachine.getCurrentStateId() + "',";
                errorMsg += " role mapping -- " + mStateMachine.getRoleMapping().toString();
                errorMsg += " -- " + std::string(e.what()) + "\n";
                throw conversation::ProtocolException(errorMsg);
            }
            
            notifyAll(msg, false);
            return;
        } else if( msg.getProtocol().empty())
        {
            LOG_WARN("Conversation: received message has no protocol being set. Current conversation using '%s'", mProtocol.c_str());
        }

        if( msg.getLanguage().empty())
        {
            LOG_INFO("Conversation: received message has not language being set. Current conversation using '%s'", mContentLanguage.c_str());
        } else if(mContentLanguage != msg.getLanguage())
        {
            LOG_INFO("Conversation: message with different content language being inserted: current '%s' - to be inserted '%s'", mContentLanguage.c_str(), msg.getLanguage().c_str());
        }

        // update the message state machine
        try {
            mStateMachine.consumeMessage(msg);
        } catch(const std::runtime_error& e)
        {
            std::string errorMsg = "Conversation: unexpected message with performative '" + msg.getPerformative() + "' for the protocol '" + msg.getProtocol() + "' ";
            errorMsg += " current state: '" + mStateMachine.getCurrentStateId() + "',";
            errorMsg += " role mapping -- " + mStateMachine.getRoleMapping().toString();
            errorMsg += " -- " + std::string(e.what()) + "\n";
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
        notify(msg, conversation::END_OF_CONVERSATION);
    } else if(newConversation) {
        notify(msg, conversation::START_OF_CONVERSATION);
    } else {
        notify(msg, conversation::INTERMEDIATE_UPDATE);
    }
}

fipa::acl::ACLMessage Conversation::getLastMessage() const
{
     boost::unique_lock<boost::mutex> lock(mMutex);
     return mMessages.back();
}

bool Conversation::hasEnded() const
{
    try
    {
        if(!mStateMachine.inFinalState())
        {
            LOG_DEBUG("Conversation did not end");
            return false;
            
        }
        LOG_DEBUG("Conversation ended");
        return true;
    }
    catch(const std::runtime_error& e)
    {
        // This very probably means the state machine has not been initialized properly,
        // as there was no message yet to know the protocol. Therefore, technically
        // the conversation did not end!
        LOG_WARN_S << "Runtime error when testing if conversation ended. Therefore not ended. Message: " << e.what();
        return false;
    }
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

std::string Conversation::toString(const std::vector<fipa::acl::ACLMessage>& messages)
{
    std::stringstream ss;
    std::vector<fipa::acl::ACLMessage>::const_iterator cit = messages.begin();
    for(; cit != messages.end(); ++cit)
    {
        ss << cit->toString() << std::endl;
    }
    return ss.str();
}

std::string Conversation::toString() const
{
    std::stringstream ss;
    ss << "Conversation: " << std::endl;
    if(!mMessages.empty())
    {
        ss << "    id: " << mMessages.front().getConversationID();
    }
    ss << std::endl;
    ss << "    owner:              " << mOwner << std::endl;
    ss << "    protocol:           " << mProtocol << std::endl;
    ss << "    language:           " << mContentLanguage << std::endl;
    ss << "    # subconversations: " << mNumberOfSubConversations << std::endl;
    ss << "    # messages          " << mMessages.size() << std::endl;
    ss << "BEGIN " << std::endl;
    ss << toString(mMessages) << std::endl;
    ss << "END" << std::endl;

    return ss.str();
}

ConversationObservable::ConversationObservable()
    : mStatus(conversation::UNINITIALIZED)
{}

ConversationObservable::ConversationObservable(const ConversationObservable& other)
    : mConversationId(other.mConversationId)
    , mStatus(other.mStatus)
    , mObservers(other.mObservers)
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
