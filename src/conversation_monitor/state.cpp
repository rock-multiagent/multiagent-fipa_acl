
#include "state.h"
#include "statemachine.h"

#include <iostream>
#include <stdexcept>
#include <base/logging.h>

namespace fipa {
namespace acl {
    

State::State() : final(false), uid(), transitions(), subSM(), archive(), involvedAgents(), owningMachine(0)
{
}

State::State(const std::string& _uid) : final(false), uid(_uid), transitions(), subSM(), archive(), involvedAgents(), owningMachine(0)
{
}

State::~State()
{
}

void State::addTransition(Transition &t)
{
    std::vector<Transition>::iterator it;
    for (it = transitions.begin(); it != transitions.end(); it++)
    {
        if (unloadedEqual(*it,t) )
            return;
    }
    
    t.setOwningState(this);
    t.setMachine(owningMachine);
    transitions.push_back(t);
}

void State::generateDefaultTransitions()
{
        // Not adding any transition for final states
        if(getFinal())
            return; 

        std::vector<Transition>::iterator trit;
        for (trit = transitions.begin(); trit != transitions.end(); trit++)
        {
	  // we don't generate a not-understood transition for not-understood message...
	  if ( trit->getExpectedPerformative() == PerformativeTxt[ACLMessage::NOT_UNDERSTOOD] ) 
	      continue;
	  
	  Transition t; 
	  t.setExpectedPerformative(PerformativeTxt[ACLMessage::NOT_UNDERSTOOD]);
	  t.setFrom(trit->getTo());
	  t.setTo(trit->getFrom());
	  
	  t.setNextStateName(StateMachine::NOT_UNDERSTOOD);
	  addTransition(t);	  
        }
}

int State::consumeMessage(const ACLMessage &msg)
{
    LOG_DEBUG("State::consumeMessage state %s", uid.c_str());
    if (!subSM.empty())
    {
        LOG_DEBUG("Checking the sub-state machines");
        bool found_one = false;
        bool stillActiveSubSM = false;
        std::vector<StateMachine>::iterator smit;
        for (smit = subSM.begin(); smit != subSM.end(); smit++)
        {
	  if (smit->isActive() )
          {
	      if (smit->isConversationOver() )
              {
                  continue;
	      } else { 
                  stillActiveSubSM = true;
		  if (smit->consumeMessage(msg) == 0) 
		  {
		      found_one = true;
		  } 
              }
	  } else if (smit->startMachine(msg) == 0)
          {
              found_one = true;
          }
        }

        if (stillActiveSubSM)
        {
	  if (found_one)
              return 0;
	  else
              return 1;
        }
    } 

    std::vector<Transition>::iterator it;
    for (it = transitions.begin(); it != transitions.end(); it++)
    {
        LOG_DEBUG("Forwarding message in machine %p to transition from state %s to %s", owningMachine, uid.c_str(), it->getNextStateName().c_str());
        if (it->consumeMessage(msg) == 0) 
            return 0;
    }
    return 1;
}

void State::tickInvolvedAgent(const AgentID& ag)
{
    std::map<AgentID,bool>::iterator it;
    for (it = involvedAgents.begin(); it != involvedAgents.end(); it++)
    {
        if (it->first == ag) it->second = true;
    }
}

void State::tickInvolvedAgent(const std::vector<AgentID>& agents)
{
    std::vector<AgentID>::const_iterator it;
    for (it = agents.begin(); it != agents.end(); it++)
    {
        tickInvolvedAgent(*it);
    }
}

bool State::checkAllAgentsAccountedFor() const
{
    std::map<AgentID,bool>::const_iterator it;
    for (it = involvedAgents.begin(); it != involvedAgents.end(); it++)
    {    
        if (it->second == false) return false;
    }
    return true;
}
void State::updateInvolvedAgentsMap(Transition &it)
{
    std::vector<AgentID> temp = it.getExpectedSenders();
    std::vector<AgentID>::iterator agit;
    for (agit = temp.begin(); agit != temp.end(); agit++)
    {
        std::map<AgentID,bool>::iterator found;
        if ( (found = involvedAgents.find((*agit) ) ) == involvedAgents.end() )
        {
	  std::pair<AgentID,bool> mypair = std::pair<AgentID,bool>(*agit,false);
          LOG_INFO("Insert agent into involved agents map: %s", agit->getName().c_str());
	  involvedAgents.insert(mypair);
        }
    }
}
void State::loadParameters()
{
    LOG_INFO("Load parameters for state: %s", uid.c_str());
    std::vector<Transition>::iterator it;
    for (it = transitions.begin(); it != transitions.end(); it++)
    {
        assert(&(*it));
        it->loadParameters();
        updateInvolvedAgentsMap(*it);
        
    }
}

void State::updateAllAgentRoles()
{
    std::vector<Transition>::iterator it;
    //std::map<AgentMapping>::iterator found;
    for (std::vector<StateMachine>::iterator smit = subSM.begin(); smit != subSM.end(); smit++)
    {
        std::vector<RoleCorrelation> tempRC = smit->getRoleCorrelation();
        for (std::vector<RoleCorrelation>::iterator rcit = tempRC.begin(); rcit != tempRC.end(); rcit++)
        {
	  if(owningMachine->checkIfRoleSet(rcit->master) && !rcit->check)
	  {
	      smit->updateAllAgentRoles(rcit->master, owningMachine->getAgentsAssignedTo(rcit->master));
	  }
        }
    }
    
    for (it = transitions.begin(); it != transitions.end(); it++)
    {    
        it->updateRoles();
        updateInvolvedAgentsMap(*it);
    }
    
}
void State::setFinal(const bool _final)
{
    final = _final;
}
bool State::getFinal() const
{
    return final;
}
void State::resetInvolvedAgentsTicks()
{
    std::map<AgentID,bool>::iterator it;
    for (it = involvedAgents.begin(); it != involvedAgents.end(); it++)
    {
        it->second = false;
    }
}
void State::setAllPrecedingStates(State *st)
{
    std::vector<Transition>::iterator it;
    for (it = transitions.begin(); it != transitions.end(); it++)
    {
        it->setPrecedingState(st);
    }
}

ACLMessage* State::searchArchiveBySenderReceiver(const AgentID& m1, const AgentID& m2)
{
    std::vector<ACLMessage>::iterator it;
    for (it = archive.begin(); it != archive.end(); it++)
    {
        std::vector<AgentID> recep = it->getAllReceivers();
        if ( !it->getAllReplyTo().empty() )
        {
	  std::vector<AgentID> repto = it->getAllReplyTo();
	  if ( (find(repto.begin(), repto.end(), m2) != repto.end()) && (find(recep.begin(),recep.end(),m1) != recep.end()) )
	      return &(*it);
        }
        if ( (it->getSender() == m2) && (find(recep.begin(),recep.end(),m1) != recep.end()) ) return &(*it);
    }
    return NULL;
}

void State::setUID(const std::string& _uid)
{
    uid = _uid;
}

std::string State::getUID() const
{
    return uid;
}

void State::addToArchive(const ACLMessage &msg)
{
    archive.push_back(msg);
}

bool operator==(const State &a,const std::string &b)
{
    if (a.getUID().compare(b) ) return false;
    return true;
}
bool operator<(const AgentID &a,const AgentID &b)
{
    return true;
}


void State::setOwningMachine(StateMachine *_machine)
{
    owningMachine = _machine;

    updateTransitions();
}

void State::updateTransitions()
{
    std::vector<Transition>::iterator it = transitions.begin();
    for(; it != transitions.end(); it++)
    {
        it->setOwningState(this);
        it->setMachine(owningMachine);
    }
}

std::vector<ACLMessage> State::getMessageArchive() const	{return archive;}
std::vector<Transition> State::getTransitions() 	const	{return transitions;}
std::map<AgentID,bool> State::getInvolvedAgents() const	{return involvedAgents;}
std::vector<StateMachine> State::getSubSM() const		{return subSM;}
StateMachine* State::getOwningMachine() const	 	{return owningMachine;}

void State::print()
{
    std::cout<<"\t*********** State ***********\n";
    
    std::cout<<"\tUID: "<<uid<<"\n";
    std::cout<<"\tfinal: "<<final<<"\n";
    
    std::cout<<"\tTransitions:\n";
    for (std::vector<Transition>::iterator it = transitions.begin(); it != transitions.end(); it ++)
        it->print();
    std::cout<<"\t*****************************\n";
}



} // end of acl
} // end of fipa
