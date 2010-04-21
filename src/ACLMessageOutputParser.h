/**
 *
 * \file ACLMessageOutputParser.h
 * \author Mircea Cretu Stancu
 * \brief Encodes a given ACLMessage according to the fipa Bit-Efficent encoding speciffication(FIPA at http://www.fipa.org).
 * 
 * \version 1.0
 *  - can encode any message accordingly
 *  - as future practical requirements may demand more functionality may be added; currently, some productions of the encoding grammar are not  implemented as I could not identify the practical restraints and scenarios that may demand them;
 *  - as more of the fipa multi-agent systems module speciffications will be implemented, these productions may need to be implemented
 *  - for the actual places where the full functionality has not been implemented see the function comments in the .cpp file
 */
#ifndef ACLMessageOutputParser_H_
#define ACLMessageOutputParser_H_
#include<string>
#include"AgentAID.h"
#include"UserdefParam.h"
#include"ACLMessage.h"

namespace fipa {

namespace acl {

class ACLMessageOutputParser {

       private:
/**
	@msg message to be parsed

	@useCodeTables flag to determine whether we use code tables or not
	* sice the code tables are not implemented yet, it's value is initialized with 0 and is not to be changed

	@updateCodeTables flag to determine whether we update the code tables
	* it does not have any practical relevance unless useCodeTables = 1

	@version string that keeps the version of the encoder
	* needed by the speciffication; initialized to "1.0"
	* important restraint: as the speciffication states the version must be of strictly 2 digits;
	
	@res_depth a control variable that speciffies the depth of speciffing resolvers when encoding AgentAID's
	* see speciffication for details 
*/
               ACLMessage* msg;
               int useCodeTables;
               int updateCodeTables;
               std::string version;
               int res_depth;
               
       public:
		ACLMessageOutputParser();
		void setUseCodeTables(int x);
		int getUseCodeTables();
		void setUpdateCodeTables(int x);
		int getUpdateCodeTables();
		void setResolverDepth(int res);
		int getResolverDepth();
		void setVersion(std::string);
		std::string getVersion(); 		
		
		
		int printParsedMessage(std::string);		
		std::string getBitMessage();
		void setMessage(ACLMessage* a);
       
       private: // temporary, for testing purposes
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
	       std::string  getBitCodedNumberByte(std::string cn);
		
		

};


}//end of acl namespace

}// end of fipa namespace
#endif
