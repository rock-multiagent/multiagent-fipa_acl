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
#include<vector>

namespace fipa {

namespace acl {

    /**
	  \brief overloaded equality operator for AgentAID; the signature of the function was intentionally changed from the normal operator== (const type&, const type&)
	  
	  The parameters are not passed by reference on purpose so that the copy constructor of the class is called. This is necessary because 
	  in the comparison, pointer fields are modified(removed) and we don't want this to affect the original object
    */
        //extern bool operator== ( AgentAID &a,  AgentAID &b);
    
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
               std::vector<std::string> adresses;
	     /** \param resolvers: pointer to a set of AgentAIDs(also pointers) representing the resolvers of the current agent */
               std::vector<AgentAID> resolvers;
	     /** \param params: pointer to a set of UserdefParams(also pointers) representing the parameters of an agent id */
               std::vector<UserdefParam> params;
	    
	      
               
       public:
	 
	  /**
		\param resCompDepth variable which indicates up to what depth in the resolver network to compare 2 agent aids; default is 1; very not thread safe
	  */
	     static int resCompDepth;
	 
	      
/* 
	setter and getter methods are all the functionality needed so far
*/
               ~AgentAID();
	     AgentAID();
	     
	     /**
	      \brief overloaded copy constructor; provides deep-copies for all member fields
	     */
	     AgentAID(const AgentAID &a);
	     /**
	      \brief overloaded assignment operator; provides deep-copies for all member fields
	     */
	     AgentAID& operator=(const AgentAID &a);
	     AgentAID(const std::string nam);
	     
	     /**
		\brief setter and getter methods for all fields; they do not result in deep-copies assignments/retreivals, but this can be easily changed if needed through the overloaded operator which do
	     */
               std::string getName() const;
	     /**
		\brief the method checks whether the passed name string is a word or not(according to the fipa spec)
		\return 0 if successful 1 otherwise(name is un-alterred)
	      */
               int setName(const std::string nam);
	     /**
		\brief the method checks whether the passed adress string is a word or not(according to the fipa spec)
		\return 0 if successful 1 otherwise(adress is not inserted)
	      */
               int addAdress(const std::string &adr);
               std::vector<std::string> getAdresses() const;
               void addResolver(const AgentAID &aid);
               std::vector<AgentAID> getResolvers() const;
	     void deleteResolver(const AgentAID&);
               void addUserdefParam(const UserdefParam &p);
               std::vector<UserdefParam> getUserdefParams() const;
	     //static void setResCompDepth(int);
	     //static int getResCompDepth();
	
	private:
		/**
		    \brief method to be called by all constructors, that initializes the fields of the object
		*/
		void initializeFields();
};
/**
    \brief overloaded equality operator; the depth is the default one kept in the resComDepth member field 
*/
extern bool operator== (const AgentAID &a,const AgentAID &b);
/**
    \brief alternative function for equality operator; the depth can be specified through the deph param 
*/
extern bool resDepthEqual(const AgentAID &a,const AgentAID &b, int depth);

}//end of acl namespace

}// end of fipa namespace
#endif
