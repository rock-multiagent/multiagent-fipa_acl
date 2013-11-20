/**
 *
 * \file acl_message.h
 * \author Mircea Cretu Stancu
 * \brief describes the general data structure of a message in conformity with the fipa specifications. The methods are pretty straight forward getter and setter methods. The set container is used for various colective fields, in order to avoid duplicates
 * 
 */
#ifndef FIPA_ACL_ACLMESSAGE_H
#define FIPA_ACL_ACLMESSAGE_H

#include <vector>
#include <map>
#include <string>
#include <fipa_acl/message_generator/agent_id.h>
#include <fipa_acl/message_generator/userdef_param.h>
#include <base/time.h>

/**
* \brief Foundation of Physical Intelligent Agents
*/
namespace fipa {

/**
* \brief Agent Communication Language
*/
namespace acl {

   typedef std::string ConversationID;
   typedef std::string Protocol; 
   typedef std::string ContentLanguage;

   /**
    * \param illegalWordChars: string containing illegal characters according to the fipa definition of a word
    */
   const extern std::string illegalWordChars;

   /** 
    * \param illegalWordStart: string containing illegal first characters according to the fipa definition of a word
    */
   const extern std::string illegalWordStart;
    
/**
    \class ACLMessage
    \brief This class provides a representation of a message conforming to the FIPA specification SC00061
    \details The class implements the general structure of an ACLMessage. It follows the FIPA specification 
           SC00061(http://www.fipa.org/specs/fipa00061/index.html) with emphasis of what is needed in order to encode it
           (so far according to the bit efficient specification SC00069 -  http://www.fipa.org/specs/fipa00069/index.html)
           Important notice regarding using the class: The copy constructor provided and the overloaded assignment operator
           provide deep-copy methods for the class objects, HOWEVER the setter and getter methods(for the pointer fields i.e. sender) do not.
    Some fields were made pointers for reasons of using as little memory as possible at runtime; if the conclusion is reached
    that this is not needed, it should be a fairly simple task to make the class own all of its fields(even by leaving 
    the fields as pointers)with the provided deep-copy methods(just allocate memory for the owned field
    and use the asignment operator to assign the value of the passed parameter)
*/
class ACLMessage {

    friend class MessageFormat;

public:
    /**
        \enum Performative
        \brief an enum of all the predefined fipa message performatives so far, presented in the order used in the message and encoding specifications
    */
    enum Performative { ACCEPT_PROPOSAL = 0, AGREE, CANCEL, CALL_FOR_PROPOSAL, CONFIRM, DISCONFIRM, FAILURE, INFORM,
			    INFORM_IF, INFORM_REF, NOT_UNDERSTOOD, PROPAGATE, PROPOSE, PROXY, QUERY_IF, QUERY_REF, REFUSE,
			    REJECT_PROPOSAL, REQUEST, REQUEST_WHEN, REQUEST_WHENEVER, SUBSCRIBE, END_PERFORMATIVE
			  };

private:
    std::string mPerformative;
    /** pointer to the agentAID sending the message */
    AgentID mSender;
    /** pointer to a set of agentAIDs representing the intended receivers of the message; set was chosen for uniquness of  elements*/
    std::vector<AgentID> mReceivers;
    /** pointer to a set of agentAIDs representing where a reply to this message should be deliverred */
    std::vector<AgentID> mReplyTo;
    /** string representing the language used */
    std::string mLanguage;
    /** string representing the encoding (encoding of the content; not related to message encoding) */
    std::string mEncoding;
    /** string representing the ontology used for communication */
    std::string mOntology;
    /** string representing the protocol that the message is to handled by */
    std::string mProtocol;
    /** string representing the id of the conversation is part of */
    std::string mConversationId;
    /** string that is to be used by "someone" replying to this message */
    std::string mReplyWith;
    /** string representing what other message this message is a reply to */
    std::string mInReplyTo;
    /** added for ease of use and implementation of the encoding methods*/
    base::Time mReplyBy;
    /** pointer to a set of user defined parameters(also pointers); set container was used for uniqueness of elements*/
    std::vector<UserdefParam> mParameters;
    /** string representing the content of the message */
    std::string mContent;

public:  

    /**
     * \brief Default constructor
     */
    ACLMessage();
    
    /**
       \brief constructor of an ACLMessage with a predefined performative
       \param perf a predefined fipa performative(represented by its index in the perfs vector)
    */
    ACLMessage(Performative performative);

    /**
       \brief constructor of an ACLMessage with a custom performative
       \param perf string representing the custom performative(but it can just as well be one of the pre-defined ones)
    */
    ACLMessage(const std::string& perf);

    /**
     The parameters are not passed by reference on purpose so that the copy constructor of the class is called. This is necessary because 
        in the comparison, pointer fields are modified(removed) and we don't want this to affect the original object
    */
    bool operator==(const ACLMessage& other) const;

    /**
       \brief Set performative
       \throws runtime_error if performative does not exist
    */
    void setPerformative(Performative perf);

    /**
       \brief the method checks whether the passed performative string is a word or not(according to the fipa spec)
       \throws runtime_error if performative does not exist or is illegal
    */
    void setPerformative(const std::string& str);

    /**
     * Get Performative
     * \return performative
     */
    std::string getPerformative() const { return mPerformative; }

    /**
     * Add an agent id to the list of receivers
     */
    void addReceiver(const AgentID& aid);

    /**
     * Delete an agent id from the list of receivers
     */
    void deleteReceiver(const AgentID& aid);

    /**
     * Clear list of receivers
     */
    void clearReceivers();

    /**
     * Retrieve list of all receivers
     * \return List of all receivers
     */
    AgentIDList getAllReceivers() const { return mReceivers; }

    /**
     * Set list of all receivers
     */
    void setAllReceivers(const AgentIDList& receivers) { mReceivers = receivers; }

    /**
     * Add agent id to reply to list
     */
    void addReplyTo(const AgentID& aid);
    
    /**
     * Delete an agent id from the reply to list
     */
    void deleteReplyTo(const AgentID& aid);

    /**
     * Clear reply to list
     */
    void clearReplyTo();
    
    /**
     * Get reply to list
     * \return reply to list
     */
    AgentIDList getAllReplyTo() const { return mReplyTo; }

    /**
     * Set reply to list
     */
    void setAllReplyTo(const AgentIDList& replyTo) { mReplyTo = replyTo; }

    /**
     * Set reply to
     */
    void setInReplyTo(const std::string& str) { mInReplyTo = str; }

    /**
     * Get in reply to parameter
     */
    std::string getInReplyTo() const { return mInReplyTo; }

    /**
     * Set in reply with parameter
     */
    void setReplyWith(const std::string& str) { mReplyWith = str; }

    /**
     * Get reply with parameter
     */
    std::string getReplyWith() const { return mReplyWith; }

    /**
     * Set conversation id
     */
    void setConversationID(const std::string& str) { mConversationId = str; }

    /**
     * Get conversation id
     */
    std::string getConversationID() const { return mConversationId; }

     /**
       \brief the method checks whether the passed protocol string is a word or not(according to the fipa spec)
       \throws runtime_error when protocol name contains illegal characters
    */
    void setProtocol(const std::string& str);

    /**
     * Get protocol
     */
    std::string getProtocol() const { return mProtocol; }

    /**
     * Set ontology parameter
     */
    void setOntology(const std::string& str) { mOntology = str; }

    /**
     * Get ontology parameter
     */
    std::string getOntology() const { return mOntology; }

    /**
     * Set encoding
     */
    void setEncoding(const std::string& str) { mEncoding = str; }

    /**
     * Get encoding
     */
    std::string getEncoding() const { return mEncoding; }

    /**
     * Set language
     */
    void setLanguage(const std::string& str) { mLanguage = str; }

    /**
     * Get language
     */
    std::string getLanguage() const { return mLanguage; }

    /**
     * Set content 
     */
    void setContent(const std::string& content) { mContent = content; }
   
    /**
     * Check whether the content has to be treated as binary or not. This is done by simply checking on the 
     * existance of a \0 (0x00) in the data. 
     * \returns bool if the data contains at least one 0x00 markers within
     */
    bool hasBinaryContent() const;

    /**
     * Get content as string
     * If the content is binary use string's data() function to access the underlying array
     * \return content data
     */
    std::string getContent() const { return mContent; }

    /**
     * Get reference to content object in order to avoid unnecessary content copies
     */
    const std::string* getContentPtr() const { return &mContent; }

    /**
     * Set the sender of this message
     * \param sender Sender's AgentID
     */
    void setSender(const AgentID& sender) { mSender = sender; }

    /**
     * Get the senders AgentID
     * \return AgentID of the sender 
     */
    AgentID getSender() const { return mSender; }

    /**
     * Add a userdefined parameter
     * \param p userdefined parameter
     */
    void addUserdefParam(const UserdefParam& p);

    /**
     * Retrieve any userdefined parameters this message contains
     * \return List of userdefined parameters
     */
    std::vector<UserdefParam> getUserdefParams() const { return mParameters;  }

    /**
     * Set the list of userdefined parameter of this message
     * Overwrites an already existing parameter list
     */
    void setUserdefParams(const std::vector<UserdefParam>& p);

    /**
     * Return reply by as time object
     */
    base::Time getReplyBy() const { return mReplyBy; }

    /**
     * Set reply by as time object
     * \param time Time 
     */
    void setReplyBy(const base::Time& time) { mReplyBy = time; }

    /**
     * Get Performative from string
     * \throws std::runtime_error when string is not a known representation of a performative
     */
    static Performative performativeFromString(const std::string& performative);

    /**
     * Convert message to string
     * \return Message in string format
     */
    std::string toString() const;

};

extern std::map<ACLMessage::Performative, std::string> PerformativeTxt;

}//end of acl namespace

}// end of fipa namespace
#endif
