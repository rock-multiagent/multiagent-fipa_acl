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


