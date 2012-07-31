/**
 *
 * \file acl_message.h
 * \author Mircea Cretu Stancu
 * \brief describes the general data structure of a message in conformity with the fipa specifications. The methods are pretty straight forward getter and setter methods. The set container is used for various colective fields, in order to avoid duplicates
 * 
 * \version 1.0
 */
#ifndef ACLMessage_H_
#define ACLMessage_H_

#include <vector>
#include <map>
#include <string>
#include <fipa_acl/message_generator/agent_id.h>
#include <fipa_acl/message_generator/userdef_param.h>

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
    
    /*
        \brief overloaded equality operator for ACLMessage; the signature of the function was intentionally changed from the normal operator== ( type&, const type&)

        The parameters are not passed by reference on purpose so that the copy constructor of the class is called. This is necessary because 
        in the comparison, pointer fields are modified(removed) and we don't want this to affect the original object
    */
    //extern bool operator== ( ACLMessage &a,  ACLMessage &b);
        
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

    friend class ACLMessageOutputParser;

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
    std::string performative;
    /** \param sender: pointer to the agentAID sending the message */
    AgentID sender;
    /** \param receivers: pointer to a set of agentAIDs representing the intended receivers of the message; set was chosen for uniquness of  elements*/
    std::vector<AgentID> receivers;
    /** \param reply_to: pointer to a set of agentAIDs representing where a reply to this message should be deliverred */
    std::vector<AgentID> reply_to;
    /** \param language: string representing the language used */
    std::string language;
    /** \param encoding: string representing the encoding (encoding of the content; not related to message encoding) */
    std::string encoding;
    /** \param ontology: string representing the ontology used for communication */
    std::string ontology;
    /** \param protocol: string representing the protocol that the message is to handled by */
    std::string protocol;
    /** \param conversation_id: string representing the id of the conversation is part of */
    std::string conversation_id;
    /** \param reply_with: string that is to be used by "someone" replying to this message */
    std::string reply_with;
    /** \param in_reply_to: string representing what other message this message is a reply to */
    std::string in_reply_to;
    /** \param reply_by: long representing the time interval that this message should be answered by; added for fipa compliance, not the parameter used for in this purpose for the implementation so far*/
    long reply_by;
    /** \param reply_by1: string version of the reply_by parameter; added for ease of use and implementation of the encoding methods*/
    std::string reply_by1;
    /** \param params: pointer to a set of user defined parameters(also pointers); set container was used for uniqueness of elements*/
    std::vector<UserdefParam> params;
    /** \param content: string representing the content of the message */
    std::string content;


    /**
     * Set reply by using the provided string
     * No verification applies
     */
    void _setReplyBy1 (const std::string& date1);

protected:

    /**
     * Get reference to content object in order to avoid unnecessary content copies
     */
    std::string* getContentPtr();

public:  

    /**
      \brief this method does all allocations needed upon creating an ACLMessage. It is to be called by every constructor implemented 
    */
    void initializeObject();
    
    /** 
     * \brief Default deconstructor
     */
    ~ACLMessage();

    /**
     * \brief Default constructor
     */
    ACLMessage();
    
    /**
       \brief copy constructor; provides deep-copy of all fields
       \param message ACLMessage to construct from
    */
    ACLMessage(const ACLMessage &message);

    /**
       \brief overloaded assignment operator; provides deep-copy for all fields
       \param message Message to compare with
    */
    ACLMessage& operator=(const ACLMessage &message);

    /**
       \brief constructor of an ACLMessage with a predefined performative
       \param perf a predefined fipa performative(represented by its index in the perfs vector)
    */
    ACLMessage(Performative perf);

    /**
       \brief constructor of an ACLMessage with a custom performative
       \param perf string representing the custom performative(but it can just as well be one of the pre-defined ones)
    */
    ACLMessage(const std::string& perf);
           
    /**
       \brief setter and getter methods for all the fields; for fields implemented using containers have an "add" method so that we can populate them sequentially
       \throws runtime_error if performative does not exist
    */

    void setPerformative(Performative perf);
    /**
       \brief the method checks whether the passed performative string is a word or not(according to the fipa spec)
       \throws runtime_error if performative does not exist or is illegal
    */
    void setPerformative(const std::string& str);

    std::string getPerformative() const;
    void addReceiver(const AgentID& aid);
    void deleteReceiver(const AgentID& aid);
    void clearReceivers();
    std::vector<AgentID> getAllReceivers() const;
    void addReplyTo(const AgentID& aid);
    void deleteReplyTo(const AgentID& aid);
    void clearReplyTo();
    std::vector<AgentID> getAllReplyTo() const;
    void setReplyBy(long by);
    long getReplyBy() const;
    void setInReplyTo(const std::string& str);
    std::string getInReplyTo() const;
    void setReplyWith(const std::string& str);
    std::string getReplyWith() const;
    void setConversationID(const std::string& str);
    std::string getConversationID() const;

     /**
       \brief the method checks whether the passed protocol string is a word or not(according to the fipa spec)
       \throws runtime_error when protocol name contains illegal characters
    */
    void setProtocol(const std::string& str);
    std::string getProtocol() const;
    void setOntology(const std::string& str);
    std::string getOntology() const;
    void setEncoding(const std::string& str);
    std::string getEncoding() const;
    void setLanguage(const std::string& str);
    std::string getLanguage() const;
    void setContent(const std::string& cont);

   
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
    std::string getContent() const;

    /**
     * Set the sender of this message
     * \param sender Sender's AgentID
     */
    void setSender(const AgentID& sender);

    /**
     * Get the senders AgentID
     * \return AgentID of the sender 
     */
    AgentID getSender() const;

    /**
     * Add a userdefined parameter
     * \param p userdefined parameter
     */
    void addUserdefParam(const UserdefParam& p);

    /**
     * Retrieve any userdefined parameters this message contains
     * \return List of userdefined parameters
     */
    std::vector<UserdefParam> getUserdefParams() const;

    /**
     * Set the list of userdefined parameter of this message
     * Overwrites an already existing parameter list
     */
    void setUserdefParams(const std::vector<UserdefParam>& p);

    /**
     * Allow to retrieve the ReplyBy time either formatted or unformatted. 
     * Unformatted: YYYYmmddHHMMSSsss
     * Formatted: YYYY-mm-ddTHH:MM:SS:sss
       \param formatted option to get the parameter as it is stored or formated. default is formatted,call with false to get unformatted
    */
    std::string getReplyBy1(bool formatted = false) const;

    /**
     *  \brief the method checks whether the passed date string is formatted correctly or not;
     *  example of correctly formated dates are: "2010-12-23T23:12:45:100" or "2010-12-23T23:12:45" 
     *  \throws if format of date string is wrong
    */
    void setReplyBy1(const std::string& date1);

};

extern std::map<ACLMessage::Performative, std::string> PerformativeTxt;

/**
* Overloaded equals operator for ACLMessage
*/
extern bool operator== (const ACLMessage& a,const ACLMessage& b);


}//end of acl namespace

}// end of fipa namespace
#endif
