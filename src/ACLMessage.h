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

#include<set>
#include<string>
#include"AgentAID.h"
#include"UserdefParam.h"

namespace fipa {

namespace acl {

    /**
        \enum predefinedPerformaives
        \brief an enum of all the predefined fipa message perfomratives so far, presented in the order used in the message and encoding specifications
    */
    enum predefinedPerformatives{ ACCEPT_PROPOSAL = 0, AGREE, CANCEL, CALL_FOR_PROPOSAL, CONFIRM, DISCONFIRM, FAILURE, INFORM,
			    INFORM_IF, INFORM_REF, NOT_UNDERSTOOD, PROPAGATE, PROPOSE, PROXY, QUERY_IF, QUERY_REF, REFUSE,
			    REJECT_PROPOSAL, REQUEST, REQUEST_WHEN, REQUEST_WHENEVER, SUBSCRIBE
			  };
    
    
        //extern bool operator== (ACLMessage &a, ACLMessage &b);
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
        AgentAID* sender;
        /** \param receivers: pointer to a set of agentAIDs representing the intended receivers of the message; set was chosen for uniquness of  elements*/
        std::set<AgentAID*>* receivers;
        /** \param reply_to: pointer to a set of agentAIDs representing where a reply to this message should be deliverred */
        std::set<AgentAID*>* reply_to;
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
        std::set<UserdefParam*>* params;
        /** \param content: string representing the content of the message */
        std::string content;
                

       // all predefined FIPA performatives:
      /* public:  static const int ACCEPT_PROPOSAL = 0;
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
       */
		/** \param perfs: vector with all predefined message performatives as strings */
       		static const std::string perfs[];
     
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
       ACLMessage(ACLMessage &mes);
       /**
	  \brief overloaded assignment operator; provides deep-copy for all fields
       */
       ACLMessage& operator=(ACLMessage &mes);
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
	  \brief setter and getter methods for all the fields; for fields implemented using containers have an "add" method so that we can populate them sequentially
       */
       void setPerformative(std::string str);
       std::string getPerformative();
       void addReceiver(AgentAID* aid);
       void deleteReceiver(AgentAID* aid);
       void clearReceivers();
       std::set<AgentAID*>* getAllReceivers();
       void addReplyTo(AgentAID* aid);
       void deleteReplyTo(AgentAID* aid);
       void clearReplyTo();
       std::set<AgentAID*>* getAllReplyTo();
       void setReplyBy(long by);
       long getReplyBy();
       void setInReplyTo(std::string str);
       std::string getInReplyTo();
       void setReplyWith(std::string str);
       std::string getReplyWith();
       void setConversationID(std::string str);
       std::string getConversationID();
       void setProtocol(std::string str);
       std::string getProtocol();
       void setOntology(std::string str);
       std::string getOntology();
       void setEncoding(std::string str);
       std::string getEncoding();
       void setLanguage(std::string str);
       std::string getLanguage();
       void setContent(std::string cont);
       std::string getContent();
       void setSender(AgentAID* sender1);
       AgentAID* getSender();
       void addUserdefParam(UserdefParam* p);
       std::set<UserdefParam*>* getUserdefParams();
       void setUserdefParams(std::set<UserdefParam*>* p);
       
       std::string getReplyBy1();
       void setReplyBy1(std::string date1);
       
       
};


}//end of acl namespace

}// end of fipa namespace
#endif
