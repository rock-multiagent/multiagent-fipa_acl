/**
 *
 * \file ACLMessage.h
 * \author Mircea Cretu Stancu
 * \brief describes the general data structure of a message in conformity with the fipa specifications. The methods are pretty straight forward getter and setter methods. The set container is used for various colective fields, in order to avoid duplicates
 * 
 * \version 1.0
 */
#ifndef ACLMessage_H_
#define ACLMessage_H_

#include<vector>
#include<string>
#include"AgentAID.h"
#include"UserdefParam.h"
//#include "ACLMessageOutputParser.h"

namespace fipa {

namespace acl {

    /**
        \enum predefinedPerformatives
        \brief an enum of all the predefined fipa message perfomratives so far, presented in the order used in the message and encoding specifications
    */
    enum predefinedPerformatives{ ACCEPT_PROPOSAL = 0, AGREE, CANCEL, CALL_FOR_PROPOSAL, CONFIRM, DISCONFIRM, FAILURE, INFORM,
			    INFORM_IF, INFORM_REF, NOT_UNDERSTOOD, PROPAGATE, PROPOSE, PROXY, QUERY_IF, QUERY_REF, REFUSE,
			    REJECT_PROPOSAL, REQUEST, REQUEST_WHEN, REQUEST_WHENEVER, SUBSCRIBE
			  };
   /** \param illegalWordChars: string containing illegal characters according to the fipa definition of a word*/
   const extern std::string illegalWordChars;
   /** \param illegalWordStart: string containing illegal first characters according to the fipa definition of a word*/
   const extern std::string illegalWordStart;
    
    /**
        \brief overloaded equality operator for ACLMessage; the signature of the function was intentionally changed from the normal operator== ( type&, const type&)

        The parameters are not passed by reference on purpose so that the copy constructor of the class is called. This is necessary because 
        in the comparison, pointer fields are modified(removed) and we don't want this to affect the original object
    */
    //extern bool operator== ( ACLMessage &a,  ACLMessage &b);
        
/**
    \class ACLMessage
    \brief The class implements the general structure of an ACLMessage. It follows the FIPA specification 
    \brief SC00061(http://www.fipa.org/specs/fipa00061/index.html) with emphasis of what is needed in order to encode it
    \brief (so far according to the bit efficient specification SC00069 -  http://www.fipa.org/specs/fipa00069/index.html)
    
    \brief Important notice regarding using the class: The copy constructor provided and the overloaded assignment operator
    \brief provide deep-copy methods for the class objects, HOWEVER the setter and getter methods(for the pointer fields i.e. sender) do not.
    \brief Some fields were made pointers for reasons of using as little memory as possible at runtime; if the conclusion is reached
    \brief that this is not needed, it should be a fairly simple task to make the class own all of its fields(even by leaving 
    \brief the fields as pointers)with the provided deep-copy methods(just allocate memory for the owned field
    \brief and use the asignment operator to assign the value of the passed parameter)
*/

class ACLMessage {

private:
        std::string performative;
        /** \param sender: pointer to the agentAID sending the message */
        AgentAID sender;
        /** \param receivers: pointer to a set of agentAIDs representing the intended receivers of the message; set was chosen for uniquness of  elements*/
        std::vector<AgentAID> receivers;
        /** \param reply_to: pointer to a set of agentAIDs representing where a reply to this message should be deliverred */
        std::vector<AgentAID> reply_to;
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
       
                

       // all predefined FIPA performatives:
       // Stefan: Comment in again for a successfull building
 public:  
        static const int ACCEPT_PROPOSAL = 0;
   		static const int AGREE = 1;
   		static const int CANCEL = 2;
   		static const int CALL_FOR_PROPOSAL = 3;
   		static const int CONFIRM = 4;
   		static const int DISCONFIRM = 5;
   		static const int FAILURE = 6;
   		static const int INFORM = 7;
   		static const int INFORM_IF = 8;
   		static const int INFORM_REF = 9;
   		static const int NOT_UNDERSTOOD = 10;
   		static const int PROPAGATE = 11;
   		static const int PROPOSE = 12;
   		static const int PROXY = 13;
   		static const int QUERY_IF = 14;
   		static const int QUERY_REF = 15;
   		static const int REFUSE = 16;
   		static const int REJECT_PROPOSAL = 17;
   		static const int REQUEST = 18;
   		static const int REQUEST_WHEN = 19;
   		static const int REQUEST_WHENEVER = 20;
   		static const int SUBSCRIBE = 21;
       
		/** \param perfs: vector with all predefined message performatives as strings */

       		public: static const std::string perfs[];

     
public:
        /**
	  \brief this method does all allocations needed upon creating an ACLMessage. It is to be called by every constructor implemented 
        */
       void initializeObject();
       ~ACLMessage();
       ACLMessage();
      
       /**
	  \brief copy constructor; provides deep-copy of all fields
       */
       ACLMessage(const ACLMessage &mes);
       /**
	  \brief overloaded assignment operator; provides deep-copy for all fields
       */
       ACLMessage& operator=(const ACLMessage &mes);
       /**
	  \brief constructor of an ACLMessage with a predefined performative
	  \param perf: a predefined fipa performative(represented by its index in the perfs vector)
       */
       ACLMessage(predefinedPerformatives perf);
       /**
	  \brief constructor of an ACLMessage with a custom performative
	  \param perf: string representing the custom performative(but it can just as well be one of the pre-defined ones)
       */
       ACLMessage(std::string perf);
       /**
        * Stefan: Added missing declaration for existing definition.
        */
       ACLMessage(int perf);
       
       /**
	  \brief setter and getter methods for all the fields; for fields implemented using containers have an "add" method so that we can populate them sequentially
       */
       /**
	  \brief the method checks whether the passed performative string is a word or not(according to the fipa spec)
	  \return 0 if successful 1 otherwise(performative is un-alterred)
       */
       int setPerformative(const std::string str);
       std::string getPerformative() const;
       void addReceiver(const AgentAID &aid);
       void deleteReceiver(const AgentAID &aid);
       void clearReceivers();
       std::vector<AgentAID> getAllReceivers() const;
       void addReplyTo(const AgentAID &aid);
       void deleteReplyTo(const AgentAID aid);
       void clearReplyTo();
       std::vector<AgentAID> getAllReplyTo() const;
       void setReplyBy(long by);
       long getReplyBy() const;
       void setInReplyTo(const std::string str);
       std::string getInReplyTo() const;
       void setReplyWith(const std::string str);
       std::string getReplyWith() const;
       void setConversationID(const std::string str);
       std::string getConversationID() const;
        /**
	  \brief the method checks whether the passed protocol string is a word or not(according to the fipa spec)
	  \return 0 if successful 1 otherwise(protocol is un-alterred)
       */
       int setProtocol(const std::string str);
       std::string getProtocol() const;
       void setOntology(const std::string str);
       std::string getOntology() const;
       void setEncoding(const std::string str);
       std::string getEncoding() const;
       void setLanguage(const std::string str);
       std::string getLanguage() const;
       void setContent(const std::string cont);
       std::string getContent() const;
       void setSender(const AgentAID &sender1);
       AgentAID getSender() const;
       void addUserdefParam(const UserdefParam &p);
       std::vector<UserdefParam> getUserdefParams() const;
       void setUserdefParams(const std::vector<UserdefParam> p);
       /**
	  \param formated: option to get the parameter as it is stored or formated. default is formated,call with 0 to get unformatted
	  
       */
       std::string getReplyBy1(int formated = 0) const;
       /**
	  \brief the method checks whether the passed date string is formatted correctly or not;
	  example of correctly formated date: "2010-12-23T23:12:45:100" -- any separatators can be used instead of "-:T"
	  the minimum required is the date; if the following values are not speciffied they are default-ed to 0
	  \return 0 if successful 1 if length is bad 2 if wrong format
       */
       int setReplyBy1(const std::string date1);
       void _setReplyBy1 (const std::string date1);
       
       
};

extern bool operator== (const ACLMessage &a, const ACLMessage &b);


}//end of acl namespace

}// end of fipa namespace
#endif
