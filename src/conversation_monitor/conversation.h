#ifndef FIPA_ACL_CONVERSATION_H
#define FIPA_ACL_CONVERSATION_H

#include <vector>
#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <fipa_acl/message_generator/acl_message.h>
#include <fipa_acl/conversation_monitor/transition.h>
#include <fipa_acl/conversation_monitor/statemachine_factory.h>
#include <base/Time.hpp>

namespace fipa {
namespace acl {

class ConversationObserver;
class ConversationObservable;

typedef boost::shared_ptr<ConversationObserver> ConversationObserverPtr;
typedef boost::shared_ptr<ConversationObservable> ConversationObservablePtr;
typedef std::vector<ConversationObserverPtr> ConversationObserverList;

/**
 * Conversation types 
 */
namespace conversation {
    /**
     * Event classification 
     */
    enum EventType { UNKNOWN, START_OF_CONVERSATION, INTERMEDIATE_UPDATE, END_OF_CONVERSATION, FAILURE };

    /**
     * Conversation state - multiple terminal states exist here
     */
    enum Status { UNINITIALIZED, RUNNING, ENDED, FAILED };

    static const std::string EventTypeTxt[] = { "UNKNOWN", "START_OF_CONVERSATION", "INTERMEDIATE_UPDATE", "END_OF_CONVERSATION", "FAILURE" };
    static const std::string StatusTxt[] = { "UNINITIALIZED", "RUNNING", "ENDED", "FAILED" };

    /**
     * Notification event which is forwarded from a ConversationObservable to its
     * list of attached ConversationObservers
     */
    struct Event
    {
        Event();

        Event(const fipa::acl::ACLMessage& _msg, EventType _type);

        fipa::acl::ACLMessage msg;
        EventType type;
        base::Time timestamp;
    };

    struct ProtocolException : public std::runtime_error
    {
        ProtocolException()
            : std::runtime_error("fipa::acl::conversation::ProtocolException")
        {}
    
        ProtocolException(const std::string& msg)
            : std::runtime_error("fipa::acl::conversation::ProtocolException: '" + msg + "'" )
        {}
    
    };

    struct InvalidOperation : public std::runtime_error
    {
        InvalidOperation()
            : std::runtime_error("fipa::acl::conversation::InvalidOperation")
        {}
    
        InvalidOperation(const std::string& msg)
            : std::runtime_error("fipa::acl::conversation::InvalidOperation: '" + msg + "'" )
        {}
    
    };
}

/**
 * NullDeleter as utility
 * Insert null deleter to create a shared pointer to an already existing object
 */
struct NullDeleter
{
    void operator()(void const *) const {}
};

/**
 * Observer of a conversation
 * The observer can either operate in a non-blocking fashion relying on 
 * hasEvents and getNextEvent or block using waitForNextEvent 
 */
class ConversationObserver
{
    friend class ConversationObservable;
public:
    ConversationObserver();

    /**
     * Desconstructor marked virtual to create polymorphic class
     * At least one virtual method required to allow compile to take
     * care of polymorphism
     */
    virtual ~ConversationObserver() {}

    /**
     * Check if there is a next message
     */
    bool hasEvents() const;

    /**
     * Block this thread until a new message arrives
     * \return Next message as soon as it arrives
     */
    conversation::Event waitForNextEvent();

    /** 
     * Retrieve next event
     */
    bool getNextEvent(conversation::Event& msg);

    /**
     * Get the conversation ids of observables
     */
    std::vector<fipa::acl::ConversationID> getConversationIdsOfObservables() const;

private:
    /**
     * Update conversation -- direction of msg can be inferred from receiver/sender fields
     */
    void update(const conversation::Event& event);

    /**
     * Set the observable this observer is monitoring. Registration of multiple observables
     * is possible - though the observer has to make sure of the correct handling using 
     * the information contained in a conversation::Event
     */
    void registerObservable(const ConversationObservablePtr& observable);

    // Condition to wait for new event, when operating in a blocking fashion
    boost::condition mCondition;
    mutable boost::mutex mEventsMutex;

    // List of events this observer has been notified on 
    // events are stored in incoming order (however conversation::Event also contains
    // a timestamp)
    std::vector<conversation::Event> mEvents;

    mutable boost::mutex mObservablesMutex;
    // List of observables this observer is attached to
    std::vector<ConversationObservablePtr> mObservables;
};

/**
 * Base class for a conversation which can be observed by a ConversationObserver
 */
class ConversationObservable
{
public:
    /**
     * Default constructor
     */
    ConversationObservable(); 

    ConversationObservable(const ConversationObservable& other);

    /**
     * Construct conversation observable with a given id
     */
    ConversationObservable(const fipa::acl::ConversationID& conversationId);

    /**
     * Desconstructor marked virtual to create polymorphic class
     */
    virtual ~ConversationObservable() {}

    /**
     * Set conversation id 
     */
    void setConversationId(const fipa::acl::ConversationID& conversationId) { mConversationId = conversationId; }

    /**
    * Get the conversation id
    * \return Conversation id
    */
    fipa::acl::ConversationID getConversationId() const;

    /**
     * Add a conversation observer
     */
    void addObserver(const ConversationObserverPtr& observer); 

    /**
     * Remove a conversation observer
     */
    void removeObserver(const ConversationObserverPtr& observer);

    /**
     * Notify all attached observers
     */
    void notify(const fipa::acl::ACLMessage& msg, conversation::EventType eventType);

    /**
     * Get the current status of the conversation
     */
    conversation::Status getStatus() const;

    /**
     * Retrieve the observers of this conversation
     */
    ConversationObserverList getObservers() const;

    /**
     * Remove all attached observers, e.g. to break cyclic dependencies when
     * working with shared pointers 
     */
    void detachObservers();

    /**
     * Test whether there are observers
     * \return true if there are observers attached to this observable, false otherwise
     */
    bool hasObservers() const;

protected:
    /**
    * (Conversation) identifier of the conversation observable
    */
    fipa::acl::ConversationID mConversationId;

private:
    /**
     * Protect adding and removal of observers
     */
    mutable boost::mutex mObserverMutex;
    /**
     * Status of the observable - reflecting the sequence of notifications
     */
    conversation::Status mStatus;

    /**
     * Observers for this conversation
     */
    ConversationObserverList mObservers;
};


/**
* A Conversation represents a flow of messages which 
* have the conversation id and (commonly) the protocol in common
*
* A conversation can be observed and observers will be notified when the 
* conversation is updated
*/
class Conversation : public ConversationObservable
{
public:

    /**
     * Constructor of an empty conversation 
     * Owner and conversation id need to be provided
     * \param owner Owner of the conversation will be the current agent
     * \param conversationId Conversation id - If no conversation id is provided, one will automatically be generated
     */
    Conversation(const std::string& owner, const fipa::acl::ConversationID& conversationId = fipa::acl::ConversationID());

    /**
    * Constructor of a Conversation that requires an iniating message, i.e.
    * includes a call to update
    */
    Conversation(const std::string& owner, const fipa::acl::ACLMessage& initiator);

    Conversation(const Conversation& other);

    /**
    * Conversation destructor cleans also all related commands in the command controller
    */
    virtual ~Conversation();

    /**
     * Set the protocol for this conversation
     */ 
    void setProtocol(const fipa::acl::Protocol& protocol) { mProtocol = protocol; }

    /**
    * Get protocol (which is set by the initiating message)
    * \return Protocol
    */
    fipa::acl::Protocol getProtocol() const { return mProtocol; }

    /**
     * Set content language
     */
    void setContentLanguage(const fipa::acl::ContentLanguage& language) { mContentLanguage = language; }

    /**
    * Get content language (which is set by the initiating message)
    * \return ContentLanguage
    */
    fipa::acl::ContentLanguage getContentLanguage() const;
    
    /**
     * For protocols requiring a proxy/propagate behaviour, the number of sub conversations to be started
     * needs to be known in order to monitor the end of the conversation correctly. E.g. a broker would
     * set this to m, as soon as he knows that m agents will be contacted.
     * 
     * This MUST be called BEFORE an actual message of a sub conversation is sent.
     */
    void setNumberOfSubConversations(int amount) { mNumberOfSubConversations = amount; }

    /**
    * Update a conversation using an incoming message
    * \param msg incoming or outgoing message
    */
    void update(const fipa::acl::ACLMessage& msg);

    /**
     * Get all messages in order of this conversation
     */
    std::vector<fipa::acl::ACLMessage> getMessages() const { return mMessages; }

    /**
    * Get the last message of a conversation
    * \return Last message of the conversation
    */
    fipa::acl::ACLMessage getLastMessage() const;

    /**
    * Check if conversation has ended. 
    * This is closely related to the interaction protocol flow, i.e. 
    * only when the protocol has been validated the conversation will
    * end
    * \return true if conversation ended, false otherwise
    */
    bool hasEnded() const;

    /**
    * Check if this is an active conversation
    * \return true if conversation is active and has messages, false if is not active, i.e. has not started (yet)
    */
    bool hasMessages() const;

    /**
     * Get the name of the owner of this conversation, 
     * this is equal to the agent name of the system
     */
    std::string getOwner() const;

    /**
     * Create a new ConversationID
     */
    static fipa::acl::ConversationID generateConversationID(const std::string& topic = "");

    /**
     * Convert message list to string
     */
    static std::string toString(const std::vector<fipa::acl::ACLMessage>& messages);

    /**
     * Convert conversation to string
     */
    std::string toString() const;

private:
    
    /**
     * Checks if the conversation is erronoues, ended, etc. and notifies accordingly.
     */
    void notifyAll(const fipa::acl::ACLMessage& msg, bool newConversation);
    
    /**
     * Owner of the conversation
     */
    std::string mOwner;

    /** Procotol of this conversation */
    fipa::acl::Protocol mProtocol;

    /** Content language of this conversation */
    fipa::acl::ContentLanguage mContentLanguage;

    /** Number of sub conversations to be started */
    int mNumberOfSubConversations;
    
    /**
    * List of associated messages
    */
    std::vector<fipa::acl::ACLMessage> mMessages;

    mutable boost::mutex mMutex;
    
    /**
    * The statemachine associated with the current conversation, in order 
    * to verify the message flow 
    */
    fipa::acl::StateMachine mStateMachine;

    /**
    * The general state machine factory, which allows to instanciate statemachines for
    * known protocols
    */
    static fipa::acl::StateMachineFactory msStateMachineFactory;

};

typedef boost::shared_ptr<Conversation> ConversationPtr;

} // end namespace acl
} // end namespace fipa
#endif //FIPA_ACL_CONVERSATION_H
