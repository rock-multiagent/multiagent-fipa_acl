/**
 *
 * \file AgentID.h
 * \author Mircea Cretu Stancu
 * \brief Defines the AgentID class
 * 
 * \version 1.0
 *  - has very basic features, only what was needed for the ACLMessage class
 *  - may need to be expanded/derived from depending on future needs
 */
#ifndef AgentID_H_
#define AgentID_H_

#include"UserdefParam.h"
#include<string>
#include<vector>

namespace fipa {

namespace acl {

    /*
	  \brief overloaded equality operator for AgentID; the signature of the function was intentionally changed from the normal operator== (const type&, const type&)
	  
	  The parameters are not passed by reference on purpose so that the copy constructor of the class is called. This is necessary because 
	  in the comparison, pointer fields are modified(removed) and we don't want this to affect the original object
    */
        //extern bool operator== ( AgentID &a,  AgentID &b);
    
    /**
        \class AgentID
        \brief Implements the general AgentID functionality, which is present throughout the fipa specifications(FIPA at http://www.fipa.org). Functionality may need to be extended as further modules are implemented
        Important notice: the probably main field of an AgentID, the actual id, is not yet declared/implemented, as it was not need for the encoding and it was not discussed what should it be 
    */
class AgentID {

/**
	the fields are not based on the Architecture specification and utilities, but rather on the bit-Efficient encoding specification, so they have the name, values and functionality needed to implement this specification(they happen to coincide almost completely though)
*/
       private:
	     /** \param name: name of the agent*/
               std::string name;
	     /** \param addresses: pointer to a set of strings representing the addresses of the agent*/
               std::vector<std::string> addresses;
	     /** \param resolvers: pointer to a set of AgentIDs(also pointers) representing the resolvers of the current agent */
               std::vector<AgentID> resolvers;
	     /** \param params: pointer to a set of UserdefParams(also pointers) representing the parameters of an agent id */
               std::vector<UserdefParam> params;
	    
	      
               
       public:
	 
	  /**
		\param resCompDepth variable which indicates up to what depth in the resolver network to compare 2 agent aids; default is 1; very not thread safe
	  */
	     static int resCompDepth;
	      
             ~AgentID();

 	     /**
	      * \brief Default constructor
              */
	     AgentID();
	     
	     /**
	      * \brief overloaded copy constructor; provides deep-copies for all member fields
	      */
	     AgentID(const AgentID &a);

	     /**
	      * \brief overloaded assignment operator; provides deep-copies for all member fields
	      */
	     AgentID& operator=(const AgentID &a);

	     /**
	      * \brief Construct an AgentID by name
	      * \param name
              */
	     AgentID(const std::string name);

	     /* NOTE: setter and getter methods are all the functionality needed so far */
	     
	     /**
		\brief setter and getter methods for all fields; they do not result in deep-copies assignments/retreivals, but this can be easily changed if needed through the overloaded operator which do
	     */
               std::string getName() const;

	     /**
		\brief the method checks whether the passed name string is a word or not(according to the fipa spec)
                \param name Name
		\return 0 if successful 1 otherwise(name is un-alterred)
	      */
             int setName(const std::string name);

	     /**
		\brief the method checks whether the passed address string is a word or not(according to the fipa spec)
		\param adr Address
		\return 0 if successful 1 otherwise(address is not inserted)
	      */
             int addAddress(const std::string &adr);
	
		/**
		* \brief Retrieve list of addresses
 		* \return List of addresses
		*/
		std::vector<std::string> getAddresses() const;

	       /**
		* \brief Add resolver
		* \param aid Resolver identified by its agentid
		*/
		void addResolver(const AgentID &aid);
	
 	       /**
		* \brief Get list of resolvers
		* \return list of resolvers, i.e. agentids
		*/
		std::vector<AgentID> getResolvers() const;

		/**
		 * \brief Delete a resolver
		 * \param id Resolver to delete
                 */
		void deleteResolver(const AgentID& id);
		
		/**
		 * \brief Add a userdefined parameter
		 * \param p The userdefined parameter
                 */
              	void addUserdefParam(const UserdefParam &p);
 		
		/**
		 * \brief Get the all userdefined parameter
		 * \return List of userdefined parameters
		 */
               	std::vector<UserdefParam> getUserdefParams() const;

	        //static void setResCompDepth(int);
	        //static int getResCompDepth();

		/**
		* \brief Check if AgentId is empty, i.e. does not contain a name
		* \return True, if empty, False otherwise
		*/
		bool empty();
	
	private:
		/**
		    \brief method to be called by all constructors, that initializes the fields of the object
		*/
		void initializeFields();
};


/**
    \brief overloaded equality operator; the depth is the default one kept in the resComDepth member field 
*/
extern bool operator== (const AgentID &a,const AgentID &b);
/**
    \brief alternative function for equality operator; the depth can be specified through the deph param 
*/
extern bool resDepthEqual(const AgentID &a,const AgentID &b, int depth);

}//end of acl namespace

}// end of fipa namespace
#endif
