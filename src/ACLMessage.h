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
       public: static const int ACCEPT_PROPOSAL = 0;
       public: static const int AGREE = 1;
       public: static const int CANCEL = 2;
       public: static const int CALL_FOR_PROPOSAL = 3;
       public: static const int CONFIRM = 4;
       public: static const int DISCONFIRM = 5;
       public: static const int FAILURE = 6;
       public: static const int INFORM = 7;
       public: static const int INFORM_IF = 8;
       public: static const int INFORM_REF = 9;
       public: static const int NOT_UNDERSTOOD = 10;
       public: static const int PROPAGATE = 11;
       public: static const int PROPOSE = 12;
       public: static const int PROXY = 13;
       public: static const int QUERY_IF = 14;
       public: static const int QUERY_REF = 15;
       public: static const int REFUSE = 16;
       public: static const int REJECT_PROPOSAL = 17;
       public: static const int REQUEST = 18;
       public: static const int REQUEST_WHEN = 19;
       public: static const int REQUEST_WHENEVER = 20;
       public: static const int SUBSCRIBE = 21;
       
       public: static std::string perfs[22];
     /* 
     static {
       perfs [ACCEPT_PROPOSAL] = "accept-proposal";
       perfs [AGREE] = "agree";
       perfs [CANCEL] "cancel";
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
