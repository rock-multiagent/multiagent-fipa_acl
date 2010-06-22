/**
 *
 * \file AgentAID.cpp
 * \author Mircea Cretu Stancu
 * \brief Implements the general AgentAID functionality, which is present throughout the fipa specifications(FIPA at http://www.fipa.org).
 * 
 * \version 1.0
 */
#include "AgentAID.h"
#include "ACLMessage.h"
#include <iostream>
#include <algorithm>

namespace fipa {

namespace acl {

    
int AgentAID::resCompDepth = 1;
 
AgentAID::AgentAID()
{
	name = std::string();
	name.clear();
	initializeFields();
}

AgentAID::AgentAID(const std::string nam) 
{
    initializeFields();
    if ( (nam.find_first_of(illegalWordChars) != -1) || (illegalWordStart.find_first_of(nam.c_str()[0]) != -1) )
    name = nam;
    else name.clear();
}

void AgentAID::initializeFields()
{
	params.clear();
	adresses.clear();
	resolvers.clear();
}

std::string AgentAID::getName() const {return name;}

int AgentAID::setName(const std::string nam) 
{
    if ( (nam.find_first_of(illegalWordChars) != -1) || (illegalWordStart.find_first_of(nam.c_str()[0]) != -1) )
    return 1;
    name = nam; return 0;
}

int AgentAID::addAdress(const std::string &adr) 
{
    if ( (adr.find_first_of(illegalWordChars) != -1) || (illegalWordStart.find_first_of(adr.c_str()[0]) != -1) )
    return 1;
    adresses.insert(adresses.begin(),adr); return 0;
}

std::vector<std::string> AgentAID::getAdresses() const {return adresses;}

void AgentAID::addResolver(const AgentAID &aid) 
{
    if ( find(resolvers.begin(),resolvers.end(),aid) == resolvers.end() )// prevent entering duplicates
							// NOTE: this function searches for and uses the overloaded == op not the resDepthEq()
    resolvers.insert(resolvers.begin(),aid); 
}

std::vector<AgentAID> AgentAID::getResolvers() const {return resolvers;}

void AgentAID::deleteResolver(const AgentAID &aid) 
{
    std::vector<AgentAID>::iterator it;
    if ( (it = find(resolvers.begin(),resolvers.end(),aid)) != resolvers.end() )// prevent entering duplicates
							// NOTE: this function searches for and uses the overloaded == op not the resDepthEq()
    resolvers.erase(it);
    /*
    std::vector<AgentAID>::iterator it = resolvers.begin();
    for (it; it != resolvers.end(); it++)
       {
        AgentAID a = *it;
        if ( a == aid)
        {
	  resolvers.erase(it);
            break;
        }
    }
    */
}

void AgentAID::addUserdefParam(const UserdefParam &p) 
{
    params.insert(params.begin(),p); 
}

std::vector<UserdefParam> AgentAID::getUserdefParams() const {return params;}

//void AgentAID::setResCompDepth(int x) {resCompDepth = x;}
//int AgentAID::getResCompDepth() {return resCompDepth;}


bool operator== (const AgentAID &a,const  AgentAID &b)
{
   return resDepthEqual(a,b,AgentAID::resCompDepth);
    
}
  
bool resDepthEqual(const AgentAID &a,const AgentAID &b, int depth)
{
     //saving the resCompDepth variable for later restoration
    //int depthRestore = AgentAID::getResCompDepth();
    
    //AgentAID a = AgentAID(&c);
    //AgentAID b = AgentAID(&d);
    if (a.getName().compare(b.getName()))
        return false;
    
    // comparing adresses
    std::vector<std::string> addrA = a.getAdresses();
    std::vector<std::string> addrB = b.getAdresses();
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
	      
	  } else sitb++;
	  
        }
        if (!found_one) sita++;
	  
    }
    
    if (!addrA.empty())
    { return false; }
    if (!addrB.empty())
    { return false; }
    
    
    
    // only check the resolvers if the resCompDepth > 0; 
    if (depth > 0 )
    {
        // the resolvers are compared with up to resCompDepth -1 in the resolver network
        //AgentAID::setResCompDepth(depth-1);
    // comparing resolvers
    std::vector<AgentAID> agentsA = a.getResolvers();
    std::vector<AgentAID> agentsB = b.getResolvers();
    std::vector<AgentAID>::iterator ait = agentsA.begin();
    std::vector<AgentAID>::iterator bit = agentsB.begin();
    //std::vector<AgentAID>::iterator aux;
    
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
	      
	  } else bit++;
	  
        }
        if (!found_one) ait++;
	  
    }
    
    if (!agentsA.empty())
    { return false; }
    if (!agentsB.empty())
    { return false; }
    
    }
    
    // comparing userdefined parameters
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
	      pita = paramsA.begin();
	      paramsB.erase(pitb);
	      pitb = paramsB.end();
	      found_one = 1;
	      
	  } else pitb++;
	  
        }
        if (!found_one) pita++;
	  
    }
    
    if (!paramsA.empty())
    { return false; }
    if (!paramsB.empty())
    { return false; }
    
    return true;
}



AgentAID::~AgentAID()
{
    /*
    adresses->clear();
    delete adresses;
    if (params)
    {
    std::vector<UserdefParam*>::iterator it = params->begin();
    for (it; it != params->end(); it++)
        {
	  delete (*it);
	  //params->erase(it);
        }
    params->clear();
    
    }
    delete params;
    std::cout<< sizeof(params)<<"\t agent dest \n";
    if (resolvers!=NULL)
    {
    std::vector<AgentAID*>::iterator it1 = resolvers->begin();
    for (it1; it1 != resolvers->end(); it1++)
        {	
	  delete (*it1);
	  //resolvers->erase(it1);
        }
    resolvers->clear();
    
    }
    delete resolvers;
    */
}


AgentAID::AgentAID(const AgentAID &aid)
{
    initializeFields();
    
    name = aid.getName();
   if (!aid.getAdresses().empty())
   {
        std::vector<std::string> aidAdr = aid.getAdresses();
        std::vector<std::string>::iterator otherSit = aidAdr.begin();
        for (otherSit; otherSit != aidAdr.end(); otherSit++)
        {
	  std::string mystring = std::string(*otherSit);
	  adresses.insert(adresses.begin(),mystring);
        }
   }
   if (!aid.getResolvers().empty())
   {
        
        std::vector<AgentAID> aidres = aid.getResolvers();
        std::vector<AgentAID>::iterator aidit= aidres.begin();
        for (aidit; aidit != aidres.end(); aidit++)
        {
	  AgentAID temp = AgentAID();
	  temp = (*aidit);
	  resolvers.insert(resolvers.begin(),temp);
        }
   }
    if (!aid.getUserdefParams().empty())
    { 
        std::vector<UserdefParam> aidparams = aid.getUserdefParams();
        std::vector<UserdefParam>::iterator paramit = aidparams.begin();
               
        for (paramit; paramit != aidparams.end(); paramit++)
        {
	  UserdefParam temp2 = UserdefParam();
	  temp2 = (*paramit);
	  params.insert(params.begin(),temp2);
        }
    }
}


AgentAID& AgentAID::operator=(const AgentAID &aid)
{
    
    // checking against agent1 = agent1 case
    if (this != &aid)
    {
        
        // freeing previously filled in values for adresses, userdefined parmameters and resolvers
        
        if (!params.empty())
        {
	 
	  params.clear();
        }
       
        
        if (!adresses.empty())        adresses.clear();
       
        
        if (!resolvers.empty())
        {
	 resolvers.clear();
        }
        
        
        
        initializeFields();
    
        name = aid.getName();
        if (!aid.getAdresses().empty())
        {
	  std::vector<std::string> aidAdr = aid.getAdresses();
	  std::vector<std::string>::iterator otherSit = aidAdr.begin();
	  for (otherSit; otherSit != aidAdr.end(); otherSit++)
	  {
	      std::string mystring = std::string(*otherSit);
	      adresses.insert(adresses.begin(),mystring);
	  }
        }
        if (!aid.getResolvers().empty())
        {	  
	  
	  std::vector<AgentAID> aidres = aid.getResolvers();
	  std::vector<AgentAID>::iterator aidit= aidres.begin();
	  for (aidit; aidit != aidres.end(); aidit++)
	  {
	      AgentAID temp = AgentAID();
	      temp = (*aidit);
	      resolvers.insert(resolvers.begin(),temp);
	  }	
        }
        if (!aid.getUserdefParams().empty())
        {
	  std::vector<UserdefParam> aidparams = aid.getUserdefParams();
	  std::vector<UserdefParam>::iterator paramit = aidparams.begin();
       
	  for (paramit; paramit != aidparams.end(); paramit++)
	  {
	      UserdefParam temp2 = UserdefParam();
	      temp2 = (*paramit);
	      params.insert(params.begin(),temp2);
	  }
        }
    }
    
    
    return *this;
    
}


}//end of acl namespace

}// end of fipa namespace


