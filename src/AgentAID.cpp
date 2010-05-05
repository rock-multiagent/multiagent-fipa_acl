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
	if (!(*adresses).empty()) (*adresses).clear();
	resolvers = new std::set<AgentAID*>();
	if (!(*resolvers).empty()) (*resolvers).clear();
	adresses = new std::set<std::string>();
	if (!(*adresses).empty()) (*adresses).clear();
	params = new std::set<UserdefParam*>();
	if (!(*params).empty()) (*params).clear();
}

std::string AgentAID::getName() {return name;}

void AgentAID::setName(std::string nam) {name = nam;}

void AgentAID::addAdress(std::string adr) {(*adresses).insert(adr);}

std::set<std::string>* AgentAID::getAdresses() {return adresses; }

void AgentAID::addResolver(AgentAID* aid) {(*resolvers).insert(aid); }

std::set<AgentAID*>* AgentAID::getResolvers() {return resolvers;}

void AgentAID::addUserdefParam(UserdefParam* p) {(*params).insert(p); }

std::set<UserdefParam*>* AgentAID::getUserdefParams() {return params;}

}//end of acl namespace

}// end of fipa namespace


