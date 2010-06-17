/**
 *
 * \file AgentAID.cpp
 * \author Mircea Cretu Stancu
 * \brief Implements the general AgentAID functionality, which is present throughout the fipa specifications(FIPA at http://www.fipa.org).
 * 
 * \version 1.0
 */
#include "AgentAID.h"
#include <iostream>

namespace fipa {

namespace acl {

    
    int AgentAID::resCompDepth = 1;

    /*
bool operator== (AgentAID &a, AgentAID &b)
{
    if (a.getName().compare(b.getName()))
        return false;
    
    // comparing adresses
    std::set<std::string>* addrA = a.getAdresses();
    std::set<std::string>* addrB = b.getAdresses();
    std::set<std::string>::iterator sita = addrA->begin();
    std::set<std::string>::iterator sitb = addrB->begin();
    for (sita; sita != addrA->end(); sita++)
    {
        for (sitb; sitb != addrB->end(); sitb++)
	  if (sita->comapre(sitb) == 0)
	  {
	      addrA->erase(sita);
	      addrB->erase(sitb);
	      sitb = addrB->end();
	  }
    }
    if (addrA->begin() != addrA.end())
        return false;
    if (addrB->begin() != addrB->end())
        return false;
    
    // comparing resolvers
    std::set<AgentAID*>* agentsA = a.getRezolvers();
    std::set<AgentAID*>* agentsB = b.getRezolvers();
    std::set<AgentAID*>::iterator ait = agentsA->begin();
    std::set<AgentAID*>::iterator bit = agentsB->begin();
    for (ait; ait != agentsA->end(); ait++)
    {
        for (bit; bit != agentsB->end(); bit++)
	  if ( (*(*ait)) == (*(*bit))) 
	  {
	      agentsA->erase(ait);
	      agentsB->erase(bit);
	      bit = agentsB->end();
	      
	  }
    }
    if (agentsA->begin() != agentsA->end())
        return false;
    if (agentsB->begin() != agentsB->end())
        return false;
    // comparing userdefined parameters
    std::set<UserdefParam*>* paramsA = a.getUserdefParams();
    std::set<UserdefParam*>* paramsB = b.getUserdefParams();
    std::set<UserdefParam*>::iterator pita = paramsA->begin();
    std::set<UserdefParam*>::iterator pitb = paramsB->begin();
    for (pita; pita != paramsA->end(); pita++)
    {
        for (pitb; pitb != paramsB->end(); pitb++)
	  if ( (*(*pita)) == (*(*pitb))) 
	  {
	      paramsA->erase(pita);
	      paramsB->erase(pitb);
	      pitb = paramsB->end();
	      
	  }
    }
    if (paramsA->begin() != paramsA->end())
        return false;
    if (paramsB->begin() != paramsB->end())
        return false;
    
    return true;
    
}
*/
    
AgentAID::~AgentAID()
{
    adresses->clear();
    delete adresses;
    if (params)
    {
    std::set<UserdefParam*>::iterator it = params->begin();
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
    std::set<AgentAID*>::iterator it1 = resolvers->begin();
    for (it1; it1 != resolvers->end(); it1++)
        {	
	  delete (*it1);
	  //resolvers->erase(it1);
        }
    resolvers->clear();
    
    }
    delete resolvers;
}
AgentAID::AgentAID(AgentAID &aid)
{
    initializeFields();
    
    name = aid.getName();
   if (!aid.getAdresses()->empty())
   {
        std::set<std::string> * aidAdr = aid.getAdresses();
        std::set<std::string>::iterator otherSit = aidAdr->begin();
        for (otherSit; otherSit != aidAdr->end(); otherSit++)
        {
	  std::string mystring = std::string(*otherSit);
	  adresses->insert(mystring);
        }
   }
   if (!aid.getResolvers()->empty())
   {
        
        std::set<AgentAID*>* aidres = aid.getResolvers();
        std::set<AgentAID*>::iterator aidit= aidres->begin();
        for (aidit; aidit != aidres->end(); aidit++)
        {
	  AgentAID *temp = new AgentAID();
	  *temp = (*(*aidit));
	  resolvers->insert(temp);
        }
   }
    if (!aid.getUserdefParams()->empty())
    { 
        std::set<UserdefParam*>* aidparams = aid.getUserdefParams();
        std::set<UserdefParam*>::iterator paramit = aidparams->begin();
               
        for (paramit; paramit != aidparams->end(); paramit++)
        {
	  UserdefParam *temp2 = new UserdefParam();
	  *temp2 = (*(*paramit));
	  params->insert(temp2);
        }
    }
}
    
AgentAID& AgentAID::operator=(AgentAID &aid)
{
    // checking against agent1 = agent1 case
    if (this != &aid)
    {
        
        // freeing previously filled in values for adresses, userdefined parmameters and resolvers
        
        if (!params->empty())
        {
	  std::set<UserdefParam*>::iterator it = params->begin();
	  for (it; it != params->end(); it++)
	      delete (*it);
	  params->clear();
        }
        delete params;
        
        if (!adresses->empty())        adresses->clear();
       
        
        if (!resolvers->empty())
        {
	  std::set<AgentAID*>::iterator it = resolvers->begin();
	  for (it; it != resolvers->end(); it++)
	      delete (*it);
	  resolvers->clear();
        }
        delete resolvers;
        
        
        initializeFields();
    
        name = aid.getName();
        if (!aid.getAdresses()->empty())
        {
	  std::set<std::string> * aidAdr = aid.getAdresses();
	  std::set<std::string>::iterator otherSit = aidAdr->begin();
	  for (otherSit; otherSit != aidAdr->end(); otherSit++)
	  {
	      std::string mystring = std::string(*otherSit);
	      adresses->insert(mystring);
	  }
        }
        if (!aid.getResolvers()->empty())
        {	  
	  
	  std::set<AgentAID*>* aidres = aid.getResolvers();
	  std::set<AgentAID*>::iterator aidit= aidres->begin();
	  for (aidit; aidit != aidres->end(); aidit++)
	  {
	      AgentAID *temp = new AgentAID();
	      *temp = (*(*aidit));
	      resolvers->insert(temp);
	  }	
        }
        if (!aid.getUserdefParams()->empty())
        {
	  std::set<UserdefParam*>* aidparams = aid.getUserdefParams();
	  std::set<UserdefParam*>::iterator paramit = aidparams->begin();
       
	  for (paramit; paramit != aidparams->end(); paramit++)
	  {
	      UserdefParam *temp2 = new UserdefParam();
	      *temp2 = (*(*paramit));
	      params->insert(temp2);
	  }
        }
    }
    
    
    return *this;
}
    
AgentAID::AgentAID()
{
	name = std::string();
	name.clear();
	initializeFields();
}




AgentAID::AgentAID(std::string nam) 
{
                     // name = std::string("\0");
	             initializeFields();      
                    
                     name = nam;
}

void AgentAID::initializeFields()
{
	adresses = new std::set<std::string>();
	if (!(*adresses).empty()) 
	  {
	      
	      (*adresses).clear();
	  }
	resolvers = new std::set<AgentAID*>();
	if (!(*resolvers).empty()) 
	  {
	      for (std::set<AgentAID*>::iterator it = resolvers->begin(); it != resolvers->end(); ++it)
		delete (*it);
	      (*resolvers).clear();
	  }
	params = new std::set<UserdefParam*>();
	if (!(*params).empty()) 
	  {
	      for (std::set<UserdefParam*>::iterator it = params->begin(); it != params->end(); ++it)
		delete (*it);
	      (*params).clear();
	  }
}

std::string AgentAID::getName() {return name;}

void AgentAID::setName(std::string nam) {name = nam;}

void AgentAID::addAdress(std::string adr) {(*adresses).insert(adr);}

std::set<std::string>* AgentAID::getAdresses() {return adresses; }

void AgentAID::addResolver(AgentAID* aid) 
{
    AgentAID *aid1 = new AgentAID();
    *aid1 = *aid;
    (*resolvers).insert(aid1); 
}

std::set<AgentAID*>* AgentAID::getResolvers() {return resolvers;}

void AgentAID::addUserdefParam(UserdefParam* p) 
{
    UserdefParam *p1 = new UserdefParam();
    *p1 = *p;
    (*params).insert(p1); 
}

std::set<UserdefParam*>* AgentAID::getUserdefParams() {return params;}

void AgentAID::setResCompDepth(int x) {resCompDepth = x;}
int AgentAID::getResCompDepth() {return resCompDepth;}



}//end of acl namespace

}// end of fipa namespace


