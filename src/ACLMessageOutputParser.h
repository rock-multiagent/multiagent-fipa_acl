#ifndef ACLMessageOutputParser_H_
#define ACLMessageOutputParser_H_

#include<string>
#include"AgentAID.h"
#include"UserdefParam.h"
#include"ACLMessage.h"

class ACLMessageOutputParser {

       private:
               ACLMessage* msg;
               int useCodeTables;
               int updateCodeTables;
               std::string version;
               int res_depth;
               
       public:
              ACLMessageOutputParser();
              char* getBitMessage();
       
       private:
               char getBitEndOfColl();
               std::string  getBitHeader();
               char  getBitMessageID();
               char  getBitMessageVersion();
               std::string  getBitMessageType();
               std::string  getBitBinWord(std::string sword);
               std::string  getBitMessageParameters();
               std::string  getBitPredefMessageParams();
               std::string  getBitUserdefMessageParams();
               std::string  getBitUserdefParams(std::set<UserdefParam*>* params);
               std::string  bitParseParam(UserdefParam* p);
               std::string  getBitAID(AgentAID* aid, int depth);
               std::string  getBinURLCol(std::set<std::string>* adrr);
               std::string  getBitResolvers(std::set<AgentAID*>* aids,int depth);
               std::string  getBitAIDColl(std::set<AgentAID*>* aids, int depth);
               std::string  getBitBinExpression(std::string sword,char c);
               std::string  getBitBinExpression(double n,char base);
               std::string  getBitBinNumber(double n,char base);
               std::string  getBitBinString(std::string sword);
               std::string  getBitBinString(std::string sword,int codeTables);
               std::string  getBitDigits(std::string dig);
               std::string  getBitBinDateTimeToken(std::string date1);
               std::string  getBitBinDate(std::string date1);
               std::string  getBitCodedNumber(std::string cn);

};

#endif
