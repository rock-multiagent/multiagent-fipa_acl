
#include "State.h"
#include "StateMachine.h"

#include <iostream>


namespace fipa {
namespace acl {
    

State::State()
{
    final = false;
    uid.clear();
    transitions.clear();
    subSM.clear();
    archive.clear();
    involvedAgents.clear();
}
State::State(std::string _uid)
{
    final = false;
    uid.assign(_uid);
    transitions.clear();
    subSM.clear();
    archive.clear();
    involvedAgents.clear();
}

State::~State()
{
    transitions.clear();
    subSM.clear();
    archive.clear();
    involvedAgents.clear();
}

void State::addTransition(Transition &t)
{
    std::vector<Transition>::iterator it;
    for (it = transitions.begin(); it != transitions.end(); it++)
        if (unloadedEqual(*it,t) ) return;
        //std::cout<<"\t\ttransition added to state .."<<uid<<"\n";
    t.setOwningState(this);
    if (t.getMachine() == NULL);
        t.setMachine(owningMachine);
    transitions.push_back(t);
}

void State::generateDefaultTransitions()
{
        std::vector<Transition>::iterator trit;
        for (trit = transitions.begin(); trit != transitions.end(); trit++)
        {
	  // i don't generate a not-understood transition for the not-understood message...
	  if (!trit->getExpectedPerformative().compare(ACLMessage::perfs[ACLMessage::NOT_UNDERSTOOD]) ) continue;
	  
	  //std::cout<< trit->getExpectedPerformative()<< "..exp perf for the current transition\n";
	  
	  Transition t = Transition();
	  t.setExpectedPerformative(ACLMessage::perfs[ACLMessage::NOT_UNDERSTOOD]);
	  t.setFrom(trit->getTo());
	  t.setTo(trit->getFrom());
	  
	  t.setNextStateName(StateMachine::NOT_UNDERSTOOD);
	  //std::cout<<"check2\n";
	  //t.setMessageParity(true);
	  if (!trit->getNextStateName().empty())  {(owningMachine->getStateByName(trit->getNextStateName()))->addTransition(t);}
	  else addTransition(t);
	  //std::cout<<"check1\n";
	  //std::cout<<uid<<"..state generating default trans for\n";
	  
        }
}

int State::consumeMessage(ACLMessage &msg)
{
    std::cout<<"state's\t"<<uid<<"\tconsumeMessage call\n";
    if (!subSM.empty())
    {
        std::cout<<"checking the sub-state machines..\n";
        bool found_one = false;
        bool stillActiveSubSM = false;
        std::vector<StateMachine>::iterator smit;
        for (smit = subSM.begin(); smit != subSM.end(); smit++)
        {
	  if (smit->isActive() )
	      if (smit->isConversationOver() ) continue;
	      else {  stillActiveSubSM = true;
		    if (smit->consumeMessage(msg) == 0) 
		    {
		        found_one = true;
		    } else; }
	  else if (smit->startMachine(msg) == 0) found_one = true;
	       else;
        }
        if (stillActiveSubSM)
	  if (found_one) return 0;
	  else return 1;
        else;
    }
    std::cout<<"not enterred the subSM if..\n";
    std::vector<Transition>::iterator it;
    for (it = transitions.begin(); it != transitions.end(); it++)
    {
        std::cout<<"sending mesage to a transition of state.. "<<uid<<"\n";
        if (it->consumeMessage(msg) == 0) return 0;
    }
    return 1;
}

void State::tickInvolvedAgent(AgentID ag)
{
    std::map<AgentID,bool>::iterator it;
    for (it = involvedAgents.begin(); it != involvedAgents.end(); it++)
    {
        if (it->first == ag) it->second = true;
    }
}

void State::tickInvolvedAgent(std::vector<AgentID> agents)
{
    std::vector<AgentID>::iterator it;
    for (it = agents.begin(); it != agents.end(); it++)
    {
        tickInvolvedAgent(*it);
    }
}

bool State::checkAllAgentsAccountedFor()
{
    std::map<AgentID,bool>::iterator it;
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
	  involvedAgents.insert(mypair);
        }
    }
    temp = it.getExpectedSenders();
    for (agit = temp.begin(); agit != temp.end(); agit++)
    {
        std::map<AgentID,bool>::iterator found;
        if ( (found = involvedAgents.find(*agit)) == involvedAgents.end() )
        {
	  std::pair<AgentID,bool> mypair = std::pair<AgentID,bool>(*agit,false);
	  involvedAgents.insert(mypair);
        }
    }
}
void State::loadParameters()
{
    std::vector<Transition>::iterator it;
    for (it = transitions.begin(); it != transitions.end(); it++)
    {
        it->loadParameters();
        updateInvolvedAgentsMap(*it);
        
    }
    
}

void State::updateAllAgentRoles()
{
    std::vector<Transition>::iterator it;
    //std::map<AgentMapping>::iterator found;
    for (it = transitions.begin(); it != transitions.end(); it++)
    {    
        it->updateRoles();
        updateInvolvedAgentsMap(*it);
    }
    
}
void State::setFinal(bool _final)
{
    final = _final;
}
bool State::getFinal()
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

ACLMessage* State::searchArchiveBySenderReceiver(AgentID m1,AgentID m2)
{
    std::vector<ACLMessage>::iterator it;
    for (it = archive.begin(); it != archive.end(); it++)
    {
        std::vector<AgentID> recep = it->getAllReceivers();
        if ( (it->getSender() == m1) && (find(recep.begin(),recep.end(),m2) != recep.end()) ) return &(*it);
    }
    return NULL;
}

void State::setUID(std::string _uid)
{
    uid = _uid;
}
std::string State::getUID() const
{
    return uid;
}

void State::addToArchive(ACLMessage &msg)
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

StateMachine* State::getOwningMachine()
{
    return owningMachine;
}

void State::setOwningMachine(StateMachine *_machine)
{
    owningMachine = _machine;
}

} // end of acl
} // end of fipa
