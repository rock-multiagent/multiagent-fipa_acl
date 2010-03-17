#ifndef ACLMessage_H_
#define ACLMessage_H_

#include<set>
#include<string>
#include"AgentAID.h"
#include"UserdefParam.h"

class ACLMessage {

private:
        std::string performative;
        /*
          The sender, receiver and reply_to fields should be redefined by a an implemented agend id(AID) type.
          AID generation an allocation ought to be attributes of the AMS system of the agent platform.
          For testing and preliminary use std::string will be used(or int)
        */
        AgentAID* sender;
        std::set<AgentAID*>* receivers;
        std::set<AgentAID*>* reply_to;
        std::string language;
        std::string encoding;
        std::string ontology;
        std::string protocol;
        std::string conversation_id;
        std::string reply_with;
        std::string in_reply_to;
        long reply_by;
        std::string reply_by1;
        std::set<UserdefParam*>* params;
        
        std::string content;
                

       // all predefined FIPA performatives:
       public:  static const int ACCEPT_PROPOSAL = 0;
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
       
       		static const std::string perfs[];
     /* 
     static {
       perfs [ACCEPT_PROPOSAL] = "accept-proposal";
       perfs [AGREE] = "agree";
       perfs [CANCEL] = "cancel";
       perfs [CALL_FOR_PROPOSAL] = "cfp";
       perfs [CONFIRM] = "confirm";
       perfs [DISCONFIRM] = "disconfirm";
       perfs [FAILURE] = "failure";
       perfs [INFORM] = "inform";
       perfs [INFORM_IF] = "inform-if";
       perfs [INFORM_REF] = "inform-ref";
       perfs [NOT_UNDERSTOOD] = "not-understood";
       perfs [PROPAGATE] = "propagate";
       perfs [PROPOSE] = "propose";
       perfs [PROXY] = "proxy";
       perfs [QUERY_IF] = "query-if";
       perfs [QUERY_REF] = "query-ref";
       perfs [REFUSE] = "refuse";
       perfs [REJECT_PROPOSAL] = "reject-proposal";
       perfs [REQUEST] = "request";
       perfs [REQUEST_WHEN] = "request-when";
       perfs [REQUEST_WHENEVER] = "request-whenever";
       perfs [SUBSCRIBE] =  "subscribe";
       };   
*/
public:
	void initializeObject();
       ACLMessage();
       ACLMessage(int perf);
       ACLMessage(std::string perf);
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

#endif
