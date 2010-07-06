
#include "Transition.h"

namespace fipa {
namespace acl {
    
int Transition::consumeMessage(ACLMessage &msg)
{
    if (validateMessage(msg))
    {
        if (expectedPerf.compare(ACLMessage::perfs[ACLMessage::NOT_UNDERSTOOD])) return processNotUnderstood(msg);
        performWithoutStateExit(msg);
        //if ( checkAllSendersAccountedFor(msg) && checkAllRecepientsAccountedFor(msg) )
        if (owningState->checkAllAgentsAccountedFor() )
        {
	  performOnStateExit();
        } else 
	  {
	      if (nextState->getFinal()) 
		if (machine->owner == msg.getSender() ) machine->removeInterlocutor(msg.getAllReceivers() );
		else machine->removeInterlocutor(msg.getSender());
	  }
    }
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
     updateRoles(msg);
     
}
void Transition::loadParameters()
{
    nextState = machine->getStateByName(nextStateName);
    expectedSenders.clear();
    expectedReceivers.clear();
    if (machine->checkIfRoleExists(from) )
    {
        if(machine->checkIfRoleSet(from) )
        {
	  std::vector<AgentMapping>::iterator it;
	  for (it = machine->involvedAgents.begin(); it != machine->involvedAgents.end(); it++)
	  {
	      if (!it->role.compare(from) ) expectedSenders.push_back(it->agent);
	      if (it->agent == machine->owner) { removeAllSendersBut(machine->owner); it = machine->involvedAgents.end(); }
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
	      if (!it->role.compare(to) ) expectedReceivers.push_back(it->agent);
	      if (it->agent == machine->owner) { removeAllReceiversBut(machine->owner); it = machine->involvedAgents.end(); }
	  }
        } else;
	  
    } else; //TODO: throw some violation of protocol error
}
void Transition::updateRoles()
{
    expectedSenders.clear();
    expectedReceivers.clear();
    if (machine->checkIfRoleExists(from) )
    {
        if(machine->checkIfRoleSet(from) )
        {
	  std::vector<AgentMapping>::iterator it;
	  for (it = machine->involvedAgents.begin(); it != machine->involvedAgents.end(); it++)
	  {
	      if (!it->role.compare(from) ) expectedSenders.push_back(it->agent);
	      if (it->agent == machine->owner) { removeAllSendersBut(machine->owner); it = machine->involvedAgents.end(); }
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
	      if (!it->role.compare(to) ) expectedReceivers.push_back(it->agent);
	      if (it->agent == machine->owner) { removeAllReceiversBut(machine->owner); it = machine->involvedAgents.end(); }
	  }
        } else;
	 
    } else; //TODO: throw some violation of protocol error
}
bool Transition::updateRoles(ACLMessage &msg)
{
    if (!expectedSenders.empty() ) expectedSenders.clear();
    if (!expectedRecepients.empty() ) expectedReceivers.clear();
    if (machine->checkIfRoleExists(from) )
    {
        if(machine->checkIfRoleSet(from) )
        {
	  std::vector<AgentMapping>::iterator it;
	  for (it = machine->involvedAgents.begin(); it != machine->involvedAgents.end(); it++)
	  {
	      if (!it->role.compare(from) ) expectedSenders.push_back(it->agent);
	      if (it->agent == machine->owner) { removeAllSendersBut(machine->owner); it = machine->involvedAgents.end(); }
	  }
        } else 
	  {
	      if (!machine->setRole(from,msg.getSender())) return false;
	      machine->updateAllAgentRoles();
	  }
        
    } else; //TODO: twrow some violation of protocol error
    
    if (machine->checkIfRoleExists(to) )
    {
        if (machine->checkIfRoleSet(to) )
        {
	  std::vector<AgentMapping>::iterator it;
	  for (it = involvedAgents.begin(); it != involvedAgents.end(); it++)
	  {
	      if (!it->role.compare(to) ) expectedReceivers.push_back(it->agent);
	      if (it->agent == machine->owner) { removeAllReceiversBut(machine->owner); it = involvedAgents.end(); }
	  }
        } else
	  {
	      //std::vector<AgentAID> myvec = msg.getAllReceivers();
	      //for (std::vector<AgentAID>::iterator it = myvec.begin(); it != myvec.end())
		//machine->setRole(to,*it);
	      if (!machine->setRole(to,msg.getAllReceivers()) ) return false;
	      machine->updateAllAgentRoles();
	  }
    } else; //TODO: throw some violation of protocol error
    return true;
}
void Transition::performWithoutExit(ACLMessage &msg)
{
    owningState->addToArchive(msg);
    owningState->tickRecepients(msg.getAllReceivers());
    owningState->tickSender(msg.getSender());
    
}

void Transition::performOnStateExit(ACLMessage &msg)
{
    nextState->setAllPrecedingStates(owningState);
    resetSenderTicks();
    resetRecepientsTics();
    machine->currentState = nextState;
    
}
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

bool Transition::validateSender (ACLMessage &msg)
{
    AgentAID agent = msg.getSender();
    std::map<AgentAID,bool>::iterator found = expectedSenders.find(agent);
    if ( found != expectedSenders.end() ) (*found).second = true;
    else return false;
}

bool Transition::validateRecepients (ACLMessage &msg)
{
    std::vector<AgentAID> recepients = msg.getAllReceivers();
    std::vector<AgentAID>::iterator it;
    for (it = recepients.begin(); it != recepients.end(); it++)
    {
        std::map<AgentAID,bool>::iterator found = expectedRecepients.find(*it);
        if (found != expectedReceivers.end() ) (*found).second = true;
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
bool Transition::validateProtocol (ACLMessage &msg)
{
    if (machine->protocol.compare(msg.getProtocol()) ) return false;
    return true;
}
bool Transition::validateConversationID (ACLMessage &msg)
{
    if (machine->convid.compare(msg.getConversationID()) ) return false;
    return true;
}


unloadedEqual(Transition &a, Transition %b)
{
    if (a.getExpectedPerformative().compare(b.getExpectedPerformative()) ) return false;
    if (a.getFrom().compare(b.getFrom()) ) return false;
    if (a.getTo().compare(b.getTo()) ) return false;
    if (a.getNextStateName().compare(b.getNextStateName()) ) return false;
    
    return true;    
}

} // end of acl
} // end of fipa