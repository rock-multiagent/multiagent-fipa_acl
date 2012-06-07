/**
 * \file message_generator/agent_id.cpp
 * \author Mircea Cretu Stancu
 * \brief Implements the general AgentID functionality, which is present throughout the fipa specifications(FIPA at http://www.fipa.org).
 * 
 * \version 1.0
 */
#include "agent_id.h"
#include "acl_message.h"
#include <iostream>
#include <algorithm>
#include <base/logging.h>

namespace fipa {

namespace acl {

    
int AgentID::resCompDepth = 1;
 
AgentID::AgentID() 
    : name()
{
    initializeFields();
}

AgentID::AgentID(const std::string& nam) 
    : name(nam)
{
    initializeFields();
    if ( (nam.find_first_of(illegalWordChars) != std::string::npos) || (illegalWordStart.find_first_of(nam.c_str()[0]) != std::string::npos) )
    {
        LOG_ERROR("AgentID: name contains invalid characters - defaulting to empty name");
        name = "";
    }
}

void AgentID::initializeFields()
{
    params.clear();
    addresses.clear();
    resolvers.clear();
}

std::string AgentID::getName() const {return name;}

int AgentID::setName(const std::string& nam) 
{
    if ( (nam.find_first_of(illegalWordChars) != std::string::npos) || (illegalWordStart.find_first_of(nam.c_str()[0]) != std::string::npos) )
    {
        return 1;
    } else {
        name = nam;
    }
    return 0;
}

int AgentID::addAddress(const std::string &adr) 
{
    if ( (adr.find_first_of(illegalWordChars) != std::string::npos) || (illegalWordStart.find_first_of(adr.c_str()[0]) != std::string::npos) )
    return 1;
    addresses.insert(addresses.begin(),adr); return 0;
}

std::vector<std::string> AgentID::getAddresses() const {return addresses;}

void AgentID::addResolver(const AgentID &aid) 
{
    // prevent entering duplicates
    // NOTE: this function searches for the resolver and uses the overloaded == op not the resDepthEq()
    if ( find(resolvers.begin(),resolvers.end(),aid) == resolvers.end() )
        resolvers.insert(resolvers.begin(),aid); 
}

std::vector<AgentID> AgentID::getResolvers() const {return resolvers;}

void AgentID::deleteResolver(const AgentID &aid) 
{
    std::vector<AgentID>::iterator it;
    // prevent entering duplicates
    // NOTE: this function searches for the resolver using the overloaded == op not the resDepthEq()
    if ( (it = find(resolvers.begin(),resolvers.end(),aid)) != resolvers.end() )
        resolvers.erase(it);
}

void AgentID::addUserdefParam(const UserdefParam &p) 
{
    params.insert(params.begin(),p); 
}

std::vector<UserdefParam> AgentID::getUserdefParams() const {return params;}

//void AgentID::setResCompDepth(int x) {resCompDepth = x;}
//int AgentID::getResCompDepth() {return resCompDepth;}


bool operator== (const AgentID &a, const AgentID &b)
{
    return resDepthEqual(a,b,AgentID::resCompDepth);
}
  
bool resDepthEqual(const AgentID &a, const AgentID &b, int depth)
{
    //saving the resCompDepth variable for later restoration
    //int depthRestore = AgentID::getResCompDepth();
    
    //AgentID a = AgentID(&c);
    //AgentID b = AgentID(&d);
    if (a.getName().compare(b.getName()))
        return false;
    
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
	        if ( resDepthEqual((*ait),(*bit),depth-1)) 
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



AgentID::~AgentID()
{
}

bool AgentID::empty()
{
    return name.empty();
}

}//end of acl namespace

}// end of fipa namespace


