#ifndef AgentAID_H_
#define AgentAID_H_

#include"UserdefParam.h"
#include<string>
#include<set>

namespace fipa {

namespace acl {

class AgentAID {

       private:
               std::string name;
               std::set<std::string>* adresses;
               std::set<AgentAID*>* resolvers;
               std::set<UserdefParam*>* params;
               
       public:
               AgentAID();
	       AgentAID(std::string nam);
               std::string getName();
               void setName(std::string nam);
               void addAdress(std::string adr);
               std::set<std::string>* getAdresses();
               void addResolver(AgentAID* aid);
               std::set<AgentAID*>* getResolvers();
               void addUserdefParam(UserdefParam* p);
               std::set<UserdefParam*>* getUserdefParams();
	
	private:
		void initializeFields();
};


}//end of acl namespace

}// end of fipa namespace
#endif
