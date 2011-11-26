
#include "transition.h"
#include "state.h"
#include "statemachine.h"

#include <iostream>
#include <stdexcept>
#include <base/logging.h>

namespace fipa {
namespace acl {
    
Transition::Transition() : from(), to(), expectedPerf(), machine(0), nextState(0), owningState(0), precedingState(0), expectedSenders(), expectedRecepients()
{
    expectedRecepients.clear();
}

int Transition::consumeMessage(const ACLMessage &msg)
{
    LOG_DEBUG("owningState is: %s",owningState->getUID().c_str() );
    if( validateMessage(msg) )
    {
        //if (expectedPerf.compare(PerformativeTxt[ACLMessage::NOT_UNDERSTOOD])) return processNotUnderstood(msg);
        performWithoutStateExit(msg);
        //if ( checkAllSendersAccountedFor(msg) && checkAllRecepientsAccountedFor(msg) )
        if (owningState->checkAllAgentsAccountedFor() )
        {
	  performOnStateExit(msg);
        } else {
	      if (nextState->getFinal()) 
	      {
		AgentID a1,a2;
		a1 = machine->owner;
		a2 = msg.getSender();
		if (a1 == a2 ) machine->removeInterlocutor(msg.getAllReceivers() );
		else machine->removeInterlocutor(msg.getSender());
	      }
	}
	return 0;
    } else {
        LOG_DEBUG("Message validation failed: message does not apply to transition");
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

bool Transition::validateMessage(const ACLMessage &msg)
{
    if (!validatePerformative(msg))
    {
        LOG_DEBUG("Performative validation failed: was %s", msg.getPerformative().c_str());
        return false;
    }
    
    if (expectedSenders.empty() || expectedRecepients.empty() ) 
    {
        if (!updateRoles(msg)) 
        {
	  LOG_DEBUG("updateRoles(msg) failed");
	  return false;
        } 
    }
    
    for (std::vector<AgentID>::iterator it = expectedSenders.begin(); it != expectedSenders.end(); ++it)
        LOG_DEBUG("validate msg from %s", it->getName().c_str());
    
    for (std::vector<AgentID>::iterator it = expectedRecepients.begin(); it != expectedRecepients.end(); ++it)
        LOG_DEBUG("validate msg to %s", it->getName().c_str() );
           
    if (!validateSender(msg)) { LOG_DEBUG("Sender validation failed"); return false; }
    if (!validateRecepients(msg)) {LOG_DEBUG("Recipient validation failed"); return false; }
    
    if (!validateConvID(msg)) { LOG_DEBUG("ConversationID validation failed"); return false; }
    if (!validateProtocol(msg)) { LOG_DEBUG("Protocol validation failed"); return false; }
    if (!validateEncoding(msg)) { LOG_DEBUG("Encoding validation failed"); return false; }
    if (!validateLanguage(msg)) {LOG_DEBUG("Language validation failed"); return false; }
    if (!validateOntology(msg)) {LOG_DEBUG("Ontology validation failed"); return false; }
    if (!validateInReplyTo(msg)) {LOG_DEBUG("InReplyTo validation failed"); return false; }
    //if (!validateReplyBy(msg)) return false;

    return true;
     
}
void Transition::loadParameters()
{
    LOG_INFO("Load parameters from %s to %s nextStateName %s", from.c_str(), to.c_str(), nextStateName.c_str());
    assert(machine);
    nextState = machine->getStateByName(nextStateName);
    if(nextState)
    {
        LOG_INFO("Load parameters: next state %s", nextState->getUID().c_str());
    } else {
        LOG_WARN("State could not be found: %s", nextStateName.c_str()); 
    }
    updateRoles();
}
void Transition::updateRoles()
{
    expectedSenders.clear();
    expectedRecepients.clear();
    LOG_INFO("UpdateRoles for stateMachine %p with owner: %s", machine, machine->getOwner().getName().c_str());
    if (machine->checkIfRoleExists(from) )
    {
        if(machine->checkIfRoleSet(from) )
        {
	  
	  std::vector<AgentMapping>::iterator it;
	  for (it = machine->involvedAgents.begin(); it != machine->involvedAgents.end(); ++it)
	  {
	      if (it->role == from ) 
	      { 		
		expectedSenders.push_back(it->agent);
			
		if (it->agent == machine->owner)
                {  
		     removeAllAgentsBut(machine->owner, expectedSenders);
		     break;
                }
              }
	  }
        }
    } else {
        LOG_ERROR("Trying to use inexistent role name for 'from'");
        throw std::runtime_error("Trying to use inexistent role name for 'from'");
    }
    
        
    if (machine->checkIfRoleExists(to) )
    {
        if (machine->checkIfRoleSet(to) )
        {
	 
	  std::vector<AgentMapping>::iterator it;
	  for (it = machine->involvedAgents.begin(); it != machine->involvedAgents.end(); ++it)
	  {
	      if (it->role == to ) 
	      {
		expectedRecepients.push_back(it->agent);
		if (it->agent == machine->owner) 
		{ 
		    removeAllAgentsBut(machine->owner,expectedRecepients); break; 
		} 
	      } 
	  }
        }
    } else {
        LOG_ERROR("Trying to use inexistent role name for 'to'");
        throw std::runtime_error("Trying to use inexistent role name for 'to'");
    }
        
    /*
    LOG_DEBUG("!without message update method!\nfrom: ");
    for (std::vector<AgentID>::iterator it1 = expectedSenders.begin(); it1 != expectedSenders.end(); it1++)
        LOG_DEBUG("%s", it1->getName() );
    LOG_DEBUG("\n");
    LOG_DEBUG("to: ");
    for (std::vector<AgentID>::iterator it2 = expectedRecepients.begin(); it2 != expectedRecepients.end(); it2++)
        LOG_DEBUG("%s", it2->getName() );
    LOG_DEBUG("\n");
    */
       
}
bool Transition::updateRoles(const ACLMessage &msg)
{
    if (!expectedSenders.empty() ) expectedSenders.clear();
    if (!expectedRecepients.empty() ) expectedRecepients.clear();
    if (machine->checkIfRoleExists(from) )
    {
       
        if(machine->checkIfRoleSet(from) )
        {
	  
	  std::vector<AgentMapping>::iterator it;
	  for (it = machine->involvedAgents.begin(); it != machine->involvedAgents.end(); ++it)
	  {
	      if (!it->role.compare(from) ) 
	      {
		expectedSenders.push_back(it->agent);
		if (it->agent == machine->owner)
		{ 
		    removeAllAgentsBut(machine->owner,expectedSenders); 
		    break;
		}
		
	      }
	  }
        } else 
	  {
	      
	      if (!machine->setRole(from,msg.getSender()))
		return false;
	      /*
	      machine->updateAllAgentRoles();
	      LOG_DEBUG("!$!$!$!$ output right after update signal terminated:\n");
	      LOG_DEBUG("!with!! message update method!\tfrom: ");
	      for (std::vector<AgentID>::iterator it1 = expectedSenders.begin(); it1 != expectedSenders.end(); it1++)
		LOG_DEBUG("%s", it1->getName() );
	      LOG_DEBUG("\n");
	      LOG_DEBUG("to: ");
	      for (std::vector<AgentID>::iterator it2 = expectedRecepients.begin(); it2 != expectedRecepients.end(); it2++)
		LOG_DEBUG("%s", it2->getName() );
	      LOG_DEBUG("\n");
       
	      
	      int myc = 0;
	      std::vector<State>::iterator trcount;
	      for (trcount = (machine->states).begin(); trcount != (machine->states).end(); trcount++)
	      {
		myc += trcount->transitions.size(); 
	      }
	      LOG_DEBUG("&&&&&&&there are currently %d transitions in the SM\n", myc);
	      LOG_DEBUG("&&&&&&&there are currently states in the SM\n", machine->states.size() );
	      */
	  }
        
    } else 
    {
        LOG_ERROR("Trying to use inexistent role name from");
        throw std::runtime_error("Trying to use inexistent role name");
    }
    
    if (machine->checkIfRoleExists(to) )
    {
        if (machine->checkIfRoleSet(to) )
        {
	  std::vector<AgentMapping>::iterator it;
	  for (it = machine->involvedAgents.begin(); it != machine->involvedAgents.end(); ++it)
	  {
	      if (!it->role.compare(to) ) 
	      {
		expectedRecepients.push_back(it->agent);
		if (it->agent == machine->owner) { removeAllAgentsBut(machine->owner,expectedRecepients); break; }
		else;
	      }
	  }
        } else
	  {
	      //std::vector<AgentID> myvec = msg.getAllReceivers();
	      //for (std::vector<AgentID>::iterator it = myvec.begin(); it != myvec.end())
		//machine->setRole(to,*it);
		
	      if (!(machine->setRole(to,msg.getAllReceivers())) ) 
		return false;
	      machine->updateAllAgentRoles();
	      
	  }
    } else 
    {
        LOG_ERROR("Trying to use inexistent role name to");
        throw std::runtime_error("Trying to use inexistent role name");
    }
    
    /*
    LOG_DEBUG("!!!exit update transition with message as param%%%%%%%%\n");
    LOG_DEBUG("from: ");
    for (std::vector<AgentID>::iterator it1 = expectedSenders.begin(); it1 != expectedSenders.end(); it1++)
        LOG_DEBUG("%s", it1->getName()  );
    LOG_DEBUG("\n");
    LOG_DEBUG("to: ");
    for (std::vector<AgentID>::iterator it2 = expectedRecepients.begin(); it2 != expectedRecepients.end(); it2++)
        LOG_DEBUG("%s", it2->getName() );
    LOG_DEBUG("\n");
    */
        
    return true;
}
void Transition::performWithoutStateExit(const ACLMessage &msg)
{
    owningState->addToArchive(msg);
    owningState->tickInvolvedAgent(msg.getAllReceivers());
    owningState->tickInvolvedAgent(msg.getSender());
    
}

void Transition::performOnStateExit(const ACLMessage &msg)
{
    if (nextState == NULL) LOG_DEBUG("\t# next state is NULL\n");
    nextState->setAllPrecedingStates(owningState);
    owningState->resetInvolvedAgentsTicks();
    machine->currentState = nextState->getUID();
    LOG_INFO("Set next state %s", nextState->getUID().c_str());
}

/*
bool Transition::checkAllSendersAccountedFor(ACLMessage &msg)
{
    std::map<AgentID,bool>::iterator check;
    for (check = owningState->expectedSenders.begin(); check != owningState->expectedSenders.end(); check++)
    {
        if (check->second == false) return false;
    }
    return true;
    
}

bool Transition::checkAllRecepientsAccountedFor(ACLMessage &msg)
{
    std::map<AgentID,bool>::iterator check;
    for (check = expectedRecepients.begin(); check != expectedRecepients.end(); check++)
    {
        if (check->second == false) return false;
    }
    return true;
    
}
*/

bool Transition::validateSender (const ACLMessage &msg)
{
    AgentID agent = msg.getSender();
    
    for (std::vector<AgentID>::iterator it = expectedSenders.begin(); it != expectedSenders.end(); ++it)
        LOG_DEBUG("Expected sender %s", it->getName().c_str() );

    std::vector<AgentID>::iterator found = find(expectedSenders.begin(),expectedSenders.end(),agent);
    if ( found != expectedSenders.end() ) return true;
    return false;
}

bool Transition::validateRecepients (const ACLMessage &msg)
{
    std::vector<AgentID> recepients = msg.getAllReceivers();
    std::vector<AgentID>::iterator it;
    
    for (it = recepients.begin(); it != recepients.end(); ++it)
    {
        std::vector<AgentID>::iterator found = find(expectedRecepients.begin(), expectedRecepients.end(),*it);
        if (found == expectedRecepients.end() )
            return false;
    }
    return true;
}

bool Transition::validateInReplyTo(const ACLMessage &msg)
{
    if (msg.getInReplyTo().empty()  || precedingState == NULL) return true;

    // Checking if we actually have a message that relates to the InReplyTo field
    std::vector<AgentID> recepients = msg.getAllReceivers();

    std::vector<AgentID>::iterator it;
    ACLMessage *fromArchive;
    for (it = recepients.begin(); it != recepients.end(); ++it)
    {
        fromArchive = precedingState->searchArchiveBySenderReceiver(msg.getSender(),*it);
        // InReplyTo agent is not known
        if (!fromArchive) return false;

        // We actually have a message which trigger this InReplyTo
        if ( fromArchive->getReplyWith() == msg.getInReplyTo() )
            return true;
        
    }
   
    // No message could be found that triggered this InReplyTo 
    return false;
}

bool Transition::validatePerformative (const ACLMessage &msg)
{
    if (expectedPerf == msg.getPerformative() ) return true;
    return false;
}
bool Transition::validateOntology (const ACLMessage &msg)
{
    if (machine->ontology.empty() ) {machine->ontology = msg.getOntology(); return true;}
    if (machine->ontology == msg.getOntology() ) return true;
    return false;
}
bool Transition::validateEncoding (const ACLMessage &msg)
{
    if (machine->encoding == msg.getEncoding() ) return true;
    return false;
}
bool Transition::validateLanguage (const ACLMessage &msg)
{
    if (machine->language.empty() ) {machine->language = msg.getLanguage(); return true;}
    if (machine->language == msg.getLanguage() ) return true;
    return false;
}
bool Transition::validateProtocol (const ACLMessage &msg)
{
    if (machine->protocol.empty() ) {machine->protocol = msg.getProtocol(); return true;}
    if (machine->protocol == msg.getProtocol() ) return true;
    return false;
}
bool Transition::validateConvID (const ACLMessage &msg)
{
    if (machine->convid.empty() ) {machine->convid = msg.getConversationID(); return true;}
    if (machine->convid == msg.getConversationID()) return true;
    return false;
}

void Transition::removeAllAgentsBut(const AgentID &ag,std::vector<AgentID> &agents)
{
    bool foundAgent = false;
    std::vector<AgentID>::iterator it = agents.begin();
    for(; it != agents.end(); ++it)
    {
        if(ag == *it)
            foundAgent = true;
    }
        
    if(foundAgent)
    {
        agents.clear();
        agents.push_back(ag);
    } else {
        LOG_WARN("An agent that should not be removed from the list, does not even exist in the list");
        agents.clear();
    }
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

void Transition::setExpectedPerformative(const std::string& _performative) 	{ expectedPerf = _performative; }
void Transition::setNextStateName	(const std::string& _state) 		{ nextStateName = _state; }
void Transition::setFrom		(const std::string& _from) 		{ from = _from; }
void Transition::setTo		(const std::string& _to) 		{ to = _to; }
void Transition::setOwningState	(State* _state)			{owningState = _state;}
void Transition::setMachine		(StateMachine* _machine)		{machine = _machine;}
    
std::string 	 Transition::getExpectedPerformative() const	{return expectedPerf; }
std::string 	 Transition::getNextStateName() const		{return nextStateName; }
std::string 	 Transition::getFrom() const			{return from; }
std::string 	 Transition::getTo() const 			{return to; }
State* 		 Transition::getNextState() const 		{return nextState; }
std::vector<AgentID> Transition::getExpectedSenders() const 	{return expectedSenders; }
std::vector<AgentID> Transition::getExpectedRecepients() const	{return expectedRecepients; }
StateMachine* 	 Transition::getMachine() const		{return machine;}
State* 		 Transition::getPrecedingState() const		{return precedingState;}
State* 		 Transition::getOwningState()	const		{return owningState;}

void Transition::print()
{
    std::cout<<"\t\t^^^^^^^^^^^ Transition ^^^^^^^^^^^\n";
    std::cout<<"\t\tfrom: "<<from<<"\n";
    std::cout<<"\t\tto: "<<to<<"\n";
    std::cout<<"\t\tnextStateName: "<<nextStateName<<"\n";
    std::cout<<"\t\texpected performative: "<<expectedPerf<<"\n";
    
    std::cout<<"\t\tpreceding state: ";
    if (precedingState)
        std::cout<< precedingState->getUID()<<"\n";
    else std::cout<< "not set\n";
    
    std::cout<<"\t\towning state: ";
    if (owningState)
        std::cout<< owningState->getUID()<<"\n";
    else std::cout<< "not set\n";
    
    std::cout<<"\t\tnext state: ";
    if (nextState)
        std::cout<< nextState->getUID()<<"\n";
    else std::cout<< "not set\n";
    
    std::cout<<"\t\t^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n";
    
}



} // end of acl
} // end of fipa
