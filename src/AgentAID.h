/**
 *
 * \file AgentAID.h
 * \author Mircea Cretu Stancu
 * \brief Implements the general AgentAID functionality, which is present throughout the fipa specifications(FIPA at http://www.fipa.org).
 * 
 * \version 1.0
 *  - has very basic features, only what was needed for the ACLMessage class
 *  - may need to be expanded/derived from depending on future needs
 */
#ifndef AgentAID_H_
#define AgentAID_H_

#include"UserdefParam.h"
#include<string>
#include<set>

namespace fipa {

namespace acl {

    //extern bool operator== (AgentAID &a, AgentAID &b);
    
class AgentAID {

/*
	the fields are not based on the Architecture specification and utilities, but rather on the bit-Efficient encoding specification, so they have the name, values and functionality needed to implement this specification 
*/
       private:
               std::string name;
               std::set<std::string>* adresses;
               std::set<AgentAID*>* resolvers;
               std::set<UserdefParam*>* params;
               
       public:
/*
	setter and getter methods are all the functionality needed so far
*/
               AgentAID();
	     AgentAID(AgentAID &a);
	     AgentAID& operator=(AgentAID &a);
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
/*
	method to be called by all existing and possibly future implemented constructors, that initializes the fields of the object
*/
		void initializeFields();
};


}//end of acl namespace

}// end of fipa namespace
#endif
