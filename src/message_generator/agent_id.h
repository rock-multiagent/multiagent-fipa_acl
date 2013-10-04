/**
* \file message_generator/agent_id.h
* \author Mircea Cretu Stancu
* \author Thomas Roehr, thomas.roehr@dfki.de
* \brief Defines the AgentID class
*/
#ifndef FIPA_ACL_AGENTID_H
#define FIPA_ACL_AGENTID_H

#include <fipa_acl/message_generator/userdef_param.h>
#include <string>
#include <vector>
#include <iosfwd>
#include <utility>

namespace fipa {

namespace acl {

class AgentID;

// TODO: Recursive container, i.e. container with incomplete type are not supported by 
// standard containers (though it will work for gcc) 
// switch to using boost::container in future
typedef std::vector<AgentID> AgentIDList;

typedef AgentIDList Resolvers;
typedef std::vector<std::string> Addresses;

/**
    \class AgentID
    \brief Implements the general AgentID functionality, which is present throughout the fipa 
    specifications(FIPA at http://www.fipa.org). Functionality may need to be extended as further modules are implemented
    Important notice: the probably main field of an AgentID, the actual id, is not yet declared/implemented, as it was not 
    need for the encoding and it was not discussed what should it be 
*/
class AgentID 
{
    friend class ACLMessage;
/**
The fields are not based on the Architecture specification and utilities, but rather on the bit-Efficient encoding specification,
so they have the name, values and functionality needed to implement this specification(they happen to coincide almost completely though)
*/
private:
    /** set of strings representing the addresses of the agent*/
    std::vector<std::string> mAddresses;

    /** set of AgentIDs representing the resolvers of the current agent */
    AgentIDList mResolvers;

    /** set of UserdefParams representing the parameters of an agent id */
    std::vector<UserdefParam> mParameters;


protected:
    /** name of the agent*/
    std::string mName;

    /** name representing an undefined agent */
    static const std::string UNDEFINED;

public:

    // depth in the resolver network to compare 2 agent aids; default is 1; 
    static int msResolverComparisonDepth;

    /**
     * Retrieve an undefined agent instance
     */
    static const AgentID undefined();
    
    /**
    * \brief Default constructor
    */
    AgentID();
    
    /**
    * \brief Construct an AgentID by name
    * \param name
    */
    AgentID(const std::string& name);
    
    /* NOTE: setter and getter methods are all the functionality needed so far */
    
    /**
    \brief setter and getter methods for all fields; they do not result in deep-copies assignments/retreivals, 
    but this can be easily changed if needed through the overloaded operator which do
    */
    const std::string& getName() const { return mName; }
    
    /**
    \brief the method checks whether the passed name string is a word or not(according to the fipa spec)
    \param name Name
    \return true if successful and false otherwise(name is un-alterred)
    */
    bool setName(const std::string& name);
    
    /**
    \brief the method checks whether the passed address string is a word or not(according to the fipa spec)
    \param adr Address
    \return true if successful and false otherwise(address is not inserted)
    */
    bool addAddress(const std::string &adr);
    
    /**
    * \brief Retrieve list of addresses
    * \return List of addresses
    */
    const std::vector<std::string>& getAddresses() const { return mAddresses; }

    /**
     * \brief Set list of addresses -- overwrites existing list of addresses
     */
    void setAddresses(const std::vector<std::string>& addresses) { mAddresses = addresses; }
    
    /**
    * \brief Add resolver
    * \param aid Resolver identified by its agentid
    */
    void addResolver(const AgentID &aid);
    
    /**
    * \brief Get list of resolvers
    * \return list of resolvers, i.e. agentids
    */
    const Resolvers& getResolvers() const { return mResolvers; }

    /**
     * \brief Set list of resolvers
     */
    void setResolvers(const Resolvers& resolvers) { mResolvers = resolvers; }
    
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
    const std::vector<UserdefParam>& getUserdefParams() const { return mParameters; }

    /**
     * Set all userdefined parameters
     */
    void setUserdefParams(const std::vector<UserdefParam>& params) { mParameters = params; }
    
    //static void setResCompDepth(int);
    //static int getResCompDepth();
    
    /**
    * \brief Check if AgentId is empty, i.e. does not contain a name
    * \return True, if empty, False otherwise
    */
    bool empty();

    /**
    * \brief overloaded equality operator for AgentID; the signature of the function was intentionally changed from the normal operator== (const type&, const type&)
    * 
    * The parameters are not passed by reference on purpose so that the copy constructor of the class is called. This is necessary because 
    * in the comparison, pointer fields are modified(removed) and we don't want this to affect the original object
    */
    bool operator==(const AgentID& other) const;

    /**
      \brief alternative function for equality operator; the depth can be specified through the depth param 
    */
    static bool compareEqual(const AgentID& a, const AgentID& b, int depth);

};

template< typename C, typename E>
std::basic_ostream<C, E>& operator<<(std::basic_ostream<C,E>& out, const AgentIDList& agents)
{
    AgentIDList::const_iterator cit = agents.begin();
    out << "[";
    for(; cit != agents.end(); ++cit)
    {
        out << "AgentID<" << cit->getName() << ">";
    }
    out << "]";
    return out;
}

/**
 * Null Object for AgentID class
 */
class UndefinedAgentID : public AgentID
{
public:
    UndefinedAgentID();
};


}//end of acl namespace

}// end of fipa namespace

#endif
