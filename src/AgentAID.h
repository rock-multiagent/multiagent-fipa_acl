/**
 *
 * \file AgentAID.h
 * \author Mircea Cretu Stancu
 * \brief Defines the AgentAID class
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
    
    /**
        \class AgentAID
        \brief Implements the general AgentAID functionality, which is present throughout the fipa specifications(FIPA at http://www.fipa.org). Functionality may need to be extended as further modules are implemented
        Important notice: the probably main field of an AgentAID, the actual id, is not yet declared/implemented, as it was not need for the encoding and it was not discussed what should it be 
    */
class AgentAID {

/**
	the fields are not based on the Architecture specification and utilities, but rather on the bit-Efficient encoding specification, so they have the name, values and functionality needed to implement this specification(they happen to coincide almost completely though)
*/
       private:
	     /** \param name: name of the agent*/
               std::string name;
	     /** \param adresses: pointer to a set of strings representing the adresses of the agent*/
               std::set<std::string>* adresses;
	     /** \param resolvers: pointer to a set of AgentAIDs(also pointers) representing the resolvers of the current agent */
               std::set<AgentAID*>* resolvers;
	     /** \param params: pointer to a set of UserdefParams(also pointers) representing the parameters of an agent id */
               std::set<UserdefParam*>* params;
	     /**
		\param resCompDepth variable which indicates up to what depth in the resolver network to compare 2 agent aids; default is 1; very not thread safe
	      */
	      static int resCompDepth;
	      
               
       public:
	 
	      
/* 
	setter and getter methods are all the functionality needed so far
*/
               ~AgentAID();
	     AgentAID();
	     /**
	      \brief overloaded copy constructor; provides deep-copies for all member fields
	     */
	     AgentAID(AgentAID &a);
	     /**
	      \brief overloaded assignment operator; provides deep-copies for all member fields
	     */
	     AgentAID& operator=(AgentAID &a);
	     AgentAID(std::string nam);
	     
	     /**
		\brief setter and getter methods for all fields; they do not result in deep-copies assignments/retreivals, but this can be easily changed if needed through the overloaded operator which do
	     */
               std::string getName();
               void setName(std::string nam);
               void addAdress(std::string adr);
               std::set<std::string>* getAdresses();
               void addResolver(AgentAID* aid);
               std::set<AgentAID*>* getResolvers();
               void addUserdefParam(UserdefParam* p);
               std::set<UserdefParam*>* getUserdefParams();
	     static void setResCompDepth(int);
	     static int getResCompDepth();
	
	private:
		/**
		    \brief method to be called by all constructors, that initializes the fields of the object
		*/
		void initializeFields();
};


}//end of acl namespace

}// end of fipa namespace
#endif
