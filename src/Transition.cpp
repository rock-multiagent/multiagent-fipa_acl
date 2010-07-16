
#include "Transition.h"
#include "State.h"
#include "StateMachine.h"
#include <iostream>

namespace fipa {
namespace acl {
    
Transition::Transition()
{
    from.clear();
    to.clear();
    expectedPerf.clear();
    machine = NULL;
    owningState = NULL;
    precedingState = NULL;
    nextState = NULL;
    expectedSenders.clear();
    expectedRecepients.clear();
}
    
int Transition::consumeMessage(ACLMessage &msg)
{
    if (validateMessage(msg))
    {
        //if (expectedPerf.compare(ACLMessage::perfs[ACLMessage::NOT_UNDERSTOOD])) return processNotUnderstood(msg);
        performWithoutStateExit(msg);
        //if ( checkAllSendersAccountedFor(msg) && checkAllRecepientsAccountedFor(msg) )
        if (owningState->checkAllAgentsAccountedFor() )
        {
	  performOnStateExit(msg);
        } else 
	  {
	      if (nextState->getFinal()) 
		if (machine->owner == msg.getSender() ) machine->removeInterlocutor(msg.getAllReceivers() );
		else machine->removeInterlocutor(msg.getSender());
	  }
	  return 0;
    }
    return 1;
}

//&&& move out of a state only if all agents accounted for or ???no-one left to talk to???
/*&&& method to decide whether conv is over towith some specific other agent:
    - at each message(transition) check the "hipotetic" following state; if this is a final state then the conversation 
    between these 2 agents is done and they remove one another from the involvedAgents field of their sm
    - if the message is valid, and also the checkAll*AccountedFor() methods return true then the sm goes to the final state and that's that
    - otherwise, if the checkAll*AccountedFor() methods return false then the agent removes the other agent from the involvedAgents
    NOTE: 
*/

bool Transition::validateMessage(ACLMessage &msg)
{
    if (!validatePerformative(msg)){std::cout<<"\t\t\t\t*******1"; return false; }
    if (expectedSenders.empty() || expectedRecepients.empty() ) 
    {std::cout<<"agents not set yet..\n"; if (!updateRoles(msg)) return false; std::cout<<"agents set now..\n";}
        
    if (!validateSender(msg)) {std::cout<<"\t\t\t\t*******2"; return false; }
    if (!validateRecepients(msg)) {std::cout<<"\t\t\t\t*******3"; return false; }
    
    if (!validateConvID(msg)) {std::cout<<"\t\t\t\t*******4"; return false; }
    if (!validateProtocol(msg)) {std::cout<<"\t\t\t\t*******5"; return false; }
    if (!validateEncoding(msg)) {std::cout<<"\t\t\t\t*******6"; return false; }
    if (!validateLanguage(msg)) {std::cout<<"\t\t\t\t*******7"; return false; }
    if (!validateOntology(msg)) {std::cout<<"\t\t\t\t*******8"; return false; }
    if (!validateInReplyTo(msg)) {std::cout<<"\t\t\t\t*******9"; return false; }
    //if (!validateReplyBy(msg)) return false;
    
    
    return true;
     
}
void Transition::loadParameters()
{
    nextState = machine->getStateByName(nextStateName);
    expectedSenders.clear();
    expectedRecepients.clear();
    if (machine->checkIfRoleExists(from) )
    {
        if(machine->checkIfRoleSet(from) )
        {
	  std::vector<AgentMapping>::iterator it;
	  for (it = machine->involvedAgents.begin(); it != machine->involvedAgents.end(); it++)
	  {
	      if (!it->role.compare(from) ) expectedSenders.push_back(it->agent);
	      if (it->agent == machine->owner) { removeAllAgentsBut(machine->owner,expectedSenders); it = machine->involvedAgents.end(); }
	  }
        } else; 
	 
        
    } else; //TODO: twrow some violation of protocol error
    
    if (machine->checkIfRoleExists(to) )
    {
        if (machine->checkIfRoleSet(to) )
        {
	  std::vector<AgentMapping>::iterator it;
	  for (it = machine->involvedAgents.begin(); it != machine->involvedAgents.end(); it++)
	  {
	      if (!it->role.compare(to) ) expectedRecepients.push_back(it->agent);
	      if (it->agent == machine->owner) { removeAllAgentsBut(machine->owner,expectedRecepients); it = machine->involvedAgents.end(); }
	  }
        } else;
	  
    } else; //TODO: throw some violation of protocol error
}
void Transition::updateRoles()
{
    expectedSenders.clear();
    expectedRecepients.clear();
    if (machine->checkIfRoleExists(from) )
    {
        if(machine->checkIfRoleSet(from) )
        {
	  std::vector<AgentMapping>::iterator it;
	  for (it = machine->involvedAgents.begin(); it != machine->involvedAgents.end(); it++)
	  {
	      if (!it->role.compare(from) ) expectedSenders.push_back(it->agent);
	      if (it->agent == machine->owner) { removeAllAgentsBut(machine->owner,expectedSenders); it = machine->involvedAgents.end(); }
	  }
        } else; 
	  
    } else ; //TODO: twrow some violation of protocol error
    
    if (machine->checkIfRoleExists(to) )
    {
        if (machine->checkIfRoleSet(to) )
        {
	  std::vector<AgentMapping>::iterator it;
	  for (it = machine->involvedAgents.begin(); it != machine->involvedAgents.end(); it++)
	  {
	      if (!it->role.compare(to) ) expectedRecepients.push_back(it->agent);
	      if (it->agent == machine->owner) { removeAllAgentsBut(machine->owner,expectedRecepients); it = machine->involvedAgents.end(); }
	  }
        } else;
	 
    } else ; //TODO: throw some violation of protocol error
}
bool Transition::updateRoles(ACLMessage &msg)
{
    if (!expectedSenders.empty() ) expectedSenders.clear();
    if (!expectedRecepients.empty() ) expectedRecepients.clear();
    if (machine->checkIfRoleExists(from) )
    {
        std::cout<<"\trole to be updated exists..\n";
        if(machine->checkIfRoleSet(from) )
        {
	  std::cout<<"\trole to be updated is set..\n";
	  std::vector<AgentMapping>::iterator it;
	  for (it = machine->involvedAgents.begin(); it != machine->involvedAgents.end(); it++)
	  {
	      if (!it->role.compare(from) ) expectedSenders.push_back(it->agent);
	      if (it->agent == machine->owner) { removeAllAgentsBut(machine->owner,expectedSenders); it = machine->involvedAgents.end(); }
	  }
        } else 
	  {
	      std::cout<<"setting new role..\n";
	      if (!machine->setRole(from,msg.getSender())) return false;
	      std::cout<<"setting the new role suceeded, sending an update signal..\n";
	      machine->updateAllAgentRoles();
	      std::cout<<"roles updated with the new set role";
	  }
        
    } else return false; //TODO: twrow some violation of protocol error
    
    if (machine->checkIfRoleExists(to) )
    {
        if (machine->checkIfRoleSet(to) )
        {
	  std::vector<AgentMapping>::iterator it;
	  for (it = machine->involvedAgents.begin(); it != machine->involvedAgents.end(); it++)
	  {
	      if (!it->role.compare(to) ) expectedRecepients.push_back(it->agent);
	      if (it->agent == machine->owner) { removeAllAgentsBut(machine->owner,expectedRecepients); it = machine->involvedAgents.end(); }
	  }
        } else
	  {
	      //std::vector<AgentAID> myvec = msg.getAllReceivers();
	      //for (std::vector<AgentAID>::iterator it = myvec.begin(); it != myvec.end())
		//machine->setRole(to,*it);
	      if (!machine->setRole(to,msg.getAllReceivers()) ) return false;
	      machine->updateAllAgentRoles();
	  }
    } else return false; //TODO: throw some violation of protocol error
    return true;
}
void Transition::performWithoutStateExit(ACLMessage &msg)
{
    owningState->addToArchive(msg);
    owningState->tickInvolvedAgent(msg.getAllReceivers());
    owningState->tickInvolvedAgent(msg.getSender());
    
}

void Transition::performOnStateExit(ACLMessage &msg)
{
    nextState->setAllPrecedingStates(owningState);
    owningState->resetInvolvedAgentsTicks();
    machine->currentState = nextState;
    
}
/*
bool Transition::checkAllSendersAccountedFor(ACLMessage &msg)
{
    std::map<AgentAID,bool>::iterator check;
    for (check = owningState->expectedSenders.begin(); check != owningState->expectedSenders.end(); check++)
    {
        if (check->second == false) return false;
    }
    return true;
    
}

bool Transition::checkAllRecepientsAccountedFor(ACLMessage &msg)
{
    std::map<AgentAID,bool>::iterator check;
    for (check = expectedRecepients.begin(); check != expectedRecepients.end(); check++)
    {
        if (check->second == false) return false;
    }
    return true;
    
}
*/

bool Transition::validateSender (ACLMessage &msg)
{
    AgentAID agent = msg.getSender();
    std::vector<AgentAID>::iterator found = find(expectedSenders.begin(),expectedSenders.end(),agent);
    if ( found != expectedSenders.end() ) return true;
    return false;
}

bool Transition::validateRecepients (ACLMessage &msg)
{
    std::vector<AgentAID> recepients = msg.getAllReceivers();
    std::vector<AgentAID>::iterator it;
    for (it = recepients.begin(); it != recepients.end(); it++)
    {
        std::vector<AgentAID>::iterator found = find(expectedRecepients.begin(), expectedRecepients.end(),*it);
        if (found != expectedRecepients.end() ) ;
        else return false;
    }
    return true;
}

bool Transition::validateInReplyTo(ACLMessage &msg)
{
    ACLMessage *fromArchive;
    std::vector<AgentAID> recepients = msg.getAllReceivers();
    std::vector<AgentAID>::iterator it;
    for (it = recepients.begin(); it != recepients.end(); it++)
    {
        if ( (fromArchive = precedingState->searchArchiveBySenderReceiver(msg.getSender(),*it)) == NULL ) return false;
        //if ( (fromArchive->getReplyWith().empty() && (!msg.getInReplyTo().empty())) ||
	//   ((!fromArchive->getReplyWith().empty()) && msg.getInReplyTo().empty()) ) return false;
        if ( fromArchive->getReplyWith().compare(msg.getInReplyTo()) ) return false;
    }
    
    return true;
}

bool Transition::validatePerformative (ACLMessage &msg)
{
    
    if (expectedPerf.compare(msg.getPerformative()) ) return false;
    return true;
}
bool Transition::validateOntology (ACLMessage &msg)
{
    if (machine->ontology.empty() ) {machine->ontology = msg.getOntology(); return true;}
    if (machine->ontology.compare(msg.getOntology()) ) return false;
    return true;
}
bool Transition::validateEncoding (ACLMessage &msg)
{
    if (machine->encoding.compare(msg.getEncoding()) ) return false;
    return true;
}
bool Transition::validateLanguage (ACLMessage &msg)
{
    if (machine->encoding.empty() ) {machine->encoding = msg.getEncoding(); return true;}
    if (machine->encoding.compare(msg.getLanguage()) ) return false;
    return true;
}
bool Transition::validateProtocol (ACLMessage &msg)
{
    if (machine->protocol.empty() ) {machine->protocol = msg.getProtocol(); return true;}
    if (machine->protocol.compare(msg.getProtocol()) ) return false;
    return true;
}
bool Transition::validateConvID (ACLMessage &msg)
{
    if (machine->convid.empty() ) {machine->convid = msg.getConversationID(); return true;}
    if (machine->convid.compare(msg.getConversationID()) ) return false;
    return true;
}

void Transition::removeAllAgentsBut(AgentAID &ag,std::vector<AgentAID> &agents)
{
    agents.clear();
    agents.push_back(ag);
}



bool unloadedEqual(Transition &a, Transition &b)
{
    if (a.getExpectedPerformative().compare(b.getExpectedPerformative()) ) return false;
    if (a.getFrom().compare(b.getFrom()) ) return false;
    if (a.getTo().compare(b.getTo()) ) return false;
    if (a.getNextStateName().compare(b.getNextStateName()) ) return false;
    
    return true;    
}

void Transition::setPrecedingState(State *st)
{
    precedingState = st;
}

void Transition::setExpectedPerformative(std::string _performative) 	{ expectedPerf = _performative; }
void Transition::setNextStateName	(std::string _state) 	{ nextStateName = _state; }
void Transition::setFrom		(std::string _from) 	{ from = _from; }
void Transition::setTo		(std::string _to) 		{ to = _to; }
void Transition::setOwningState	(State* _state)		{owningState = _state;}
    
std::string Transition::getExpectedPerformative()	{return expectedPerf; }
std::string Transition::getNextStateName()	{return nextStateName; }
std::string Transition::getFrom()		{return from; }
std::string Transition::getTo()		{return to; }
State* Transition::getNextState()		{return nextState; }
std::vector<AgentAID> Transition::getExpectedSenders() 	{return expectedSenders; }
std::vector<AgentAID> Transition::getExpectedRecepients()	{return expectedRecepients; }



} // end of acl
} // end of fipa