/**
 * \file message_generator/agent_id.cpp
 * \author Mircea Cretu Stancu
 * \author Thomas Roehr, thomas.roehr@dfki.de
 * \brief Implements the general AgentID functionality, which is present throughout the fipa specifications(FIPA at http://www.fipa.org).
 */
#include "agent_id.h"
#include "acl_message.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <base-logging/Logging.hpp>

namespace fipa {
namespace acl {
    
int AgentID::msResolverComparisonDepth = 1;

const std::string AgentID::UNDEFINED = "agent_id:undefined";
 
AgentID::AgentID() 
{
    setName(AgentID::UNDEFINED);
}

AgentID::AgentID(const std::string& name)
{
    setName(name);
}

bool AgentID::isValid() const
{
    return mName != AgentID::UNDEFINED && !empty();
}

void AgentID::setName(const std::string& name)
{
    if ( (name.find_first_of(illegalWordChars) != std::string::npos) || (illegalWordStart.find_first_of(name.c_str()[0]) != std::string::npos) )
    {
        char buffer[128];
        snprintf(buffer, 128, "fipa::acl::AgentID: name '%s' contains invalid characters", name.c_str());
        throw std::runtime_error(buffer);
    }

    mName = name;
}

void AgentID::addAddress(const std::string& address)
{
    if ( (address.find_first_of(illegalWordChars) != std::string::npos) || (illegalWordStart.find_first_of(address.c_str()[0]) != std::string::npos) )
    {
        char buffer[128];
        snprintf(buffer, 128, "fipa::acl::AgentID: address '%s' contains invalid characters", address.c_str());
        throw std::runtime_error(buffer);
    }
    mAddresses.push_back(address);
}

void AgentID::addResolver(const AgentID& aid)
{
    // prevent entering duplicates
    // NOTE: this function searches for the resolver and uses the overloaded == op not the compareEqual()
    if ( find(mResolvers.begin(), mResolvers.end(),aid) == mResolvers.end() )
    {
        mResolvers.push_back(aid);
    }
}

void AgentID::deleteResolver(const AgentID& aid)
{
    std::vector<AgentID>::iterator it  = find(mResolvers.begin(), mResolvers.end(),aid);
    // prevent entering duplicates
    // NOTE: this function searches for the resolver using the overloaded == op not the compareEqual()
    if( it != mResolvers.end() )
    {
        mResolvers.erase(it);
    }
}

void AgentID::addUserdefParam(const UserdefParam& p)
{
    mParameters.push_back(p);
}

bool AgentID::operator==(const AgentID& other) const
{
    return compareEqual(*this, other, AgentID::msResolverComparisonDepth);
}
  
bool AgentID::compareEqual(const AgentID& a, const AgentID& b, int depth)
{
    if (a.getName() != b.getName())
    {
        return false;
    }
    
    // comparing addresses
    std::vector<std::string> addrA = a.getAddresses();
    std::vector<std::string> addrB = b.getAddresses();
    std::vector<std::string>::iterator sita = addrA.begin();
    std::vector<std::string>::iterator sitb = addrB.begin();
    int found_one = 0; // flag variable to control flow through inner loops
    while (sita != addrA.end())
    {
        found_one = 0;
        sitb = addrB.begin();
        while (sitb != addrB.end())
        {
            if (*sita == *sitb) 
            {
                addrA.erase(sita);
                sita = addrA.begin();
                addrB.erase(sitb);
                sitb = addrB.end();
                found_one = 1;
            } else {
                sitb++;
            }
       }

       if (!found_one)
          sita++;
    }
    
    if (!addrA.empty())
        return false;
    if (!addrB.empty())
        return false;
    
    // only check the resolvers if the resCompDepth > 0; 
    if (depth > 0 )
    {
        // the resolvers are compared with up to resCompDepth -1 in the resolver network
        //AgentID::setResCompDepth(depth-1);
        // comparing resolvers
        std::vector<AgentID> agentsA = a.getResolvers();
        std::vector<AgentID> agentsB = b.getResolvers();
        std::vector<AgentID>::iterator ait = agentsA.begin();
        std::vector<AgentID>::iterator bit = agentsB.begin();
        
        while (ait != agentsA.end())
        {
            found_one = 0;
	    bit = agentsB.begin();
	    while (bit != agentsB.end())
	    {
	        if ( compareEqual((*ait),(*bit),depth-1)) 
	        {
                     agentsA.erase(ait);
                     ait = agentsA.begin();
                     agentsB.erase(bit);
                     bit = agentsB.end();
                     found_one = 1;
                } else {
	           ++bit;
                }
            }
            if (!found_one)
               ++ait;
        }
    
        if (!agentsA.empty())
            return false; 
        if (!agentsB.empty())
            return false; 
    }
    
    // comparing userdefined parameters
    // Copy lists and delete identical elements
    // When both list are empty the compared objects are equal
    std::vector<UserdefParam> paramsA = a.getUserdefParams();
    std::vector<UserdefParam> paramsB = b.getUserdefParams();
    std::vector<UserdefParam>::iterator pita = paramsA.begin();
    std::vector<UserdefParam>::iterator pitb = paramsB.begin();
     
    while (pita != paramsA.end())
    {
        found_one = 0;
        pitb = paramsB.begin();

        while (pitb != paramsB.end())
        {
	    if (*pita == *pitb) 
	    {
                paramsA.erase(pita);
                paramsB.erase(pitb);

                pita = paramsA.begin();
	        // in order to break the inner while, set it to end()
                pitb = paramsB.end();

                found_one = 1;
            } else { 
                pitb++;
            }
        }

        if (!found_one) 
            pita++;
    }
    
    // Both lists are empty when they contained the same Parameters
    // before
    if ( paramsA.empty() && paramsB.empty())
     	return true;
    
    return false;
}

bool AgentID::empty() const
{
    return mName.empty();
}

std::string AgentID::toString(size_t indent) const
{
    std::string hspace(indent,' ');
    std::stringstream ss;
    ss << hspace << "AgentID: " << mName << std::endl;
    ss << hspace << "    addresses:" << std::endl;
    std::vector<std::string>::const_iterator ait = mAddresses.begin();
    for(; ait != mAddresses.end(); ++ait)
    {
        ss << hspace << *ait << std::endl;
    }
    ss << hspace << "    resolvers:" << std::endl;
    AgentIDList::const_iterator rit = mResolvers.begin();
    for(; rit != mResolvers.end(); ++rit)
    {
        ss << rit->toString(indent + 4) << std::endl;
    }
    ss << hspace << "    parameters:" << std::endl;
    std::vector<UserdefParam>::const_iterator pit = mParameters.begin();
    for(; pit != mParameters.end(); ++pit)
    {
        ss << hspace << pit->toString() << std::endl;
    }
    return ss.str();
}

UndefinedAgentID::UndefinedAgentID()
    : AgentID()
{
    mName = AgentID::UNDEFINED;
}

}//end of acl namespace

}// end of fipa namespace


