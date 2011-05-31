
#include "Transition.h"
#include "State.h"
#include "StateMachine.h"

#include <iostream>
#include <stdexcept>
#include <base/logging.h>

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

Transition::Transition(const Transition &t)
{
    from = t.getFrom();
    to = t.getTo();
    expectedPerf = t.getExpectedPerformative();
    nextStateName = t.getNextStateName();
    
    machine = t.getMachine();
    owningState = t.getOwningState();
    precedingState = t.getPrecedingState();
    nextState = t.getNextState();
    
    expectedSenders = t.getExpectedSenders();
    expectedRecepients = t.getExpectedRecepients();
}
    
int Transition::consumeMessage(const ACLMessage &msg)
{
    LOG_DEBUG("\towningState is:\t\t  %s\n",owningState->getUID() );
    if (validateMessage(msg))
    {
        //if (expectedPerf.compare(ACLMessage::perfs[ACLMessage::NOT_UNDERSTOOD])) return processNotUnderstood(msg);
        LOG_DEBUG("\t#### validated message\n");
        performWithoutStateExit(msg);
        //if ( checkAllSendersAccountedFor(msg) && checkAllRecepientsAccountedFor(msg) )
        LOG_DEBUG("\t#### done with perform without state exit\n");
        if (owningState->checkAllAgentsAccountedFor() )
        {
	  LOG_DEBUG("\t#### all agents accounted for\n");
	  performOnStateExit(msg);
	  LOG_DEBUG("\t#### done with perform on state exit\n");
        } else 
	  {
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
    if (!validatePerformative(msg)){LOG_DEBUG("\t\t\t\t*******1\n"); return false; }
    
    if (expectedSenders.empty() || expectedRecepients.empty() ) 
    {
        LOG_DEBUG("agents not set yet..\n");
        if (!updateRoles(msg)) 
        {
	  LOG_DEBUG("updateRoles(msg) returned 0\n");
	  return false;
        } 
        LOG_DEBUG("agents set now..\n");   
    }
    
    LOG_DEBUG("!validate message method! from: ");
    for (std::vector<AgentID>::iterator it = expectedSenders.begin(); it != expectedSenders.end(); it++)
        LOG_DEBUG("%s", it->getName());
    LOG_DEBUG("\n");
    
    LOG_DEBUG("to: ");
    for (std::vector<AgentID>::iterator it = expectedRecepients.begin(); it != expectedRecepients.end(); it++)
        LOG_DEBUG("%s", it->getName() );
    LOG_DEBUG(("\n");
    
           
    if (!validateSender(msg)) {LOG_DEBUG("\t\t\t\t*******2\n"); return false; }
    if (!validateRecepients(msg)) {LOG_DEBUG("\t\t\t\t*******3\n"); return false; }
    
    if (!validateConvID(msg)) {LOG_DEBUG("\t\t\t\t*******4\n"); return false; }
    if (!validateProtocol(msg)) {LOG_DEBUG("\t\t\t\t*******5\n"); return false; }
    if (!validateEncoding(msg)) {LOG_DEBUG("\t\t\t\t*******6\n"); return false; }
    if (!validateLanguage(msg)) {LOG_DEBUG("\t\t\t\t*******7\n"); return false; }
    LOG_DEBUG("#### passes language test\n");
    if (!validateOntology(msg)) {LOG_DEBUG("\t\t\t\t*******8\n"); return false; }
    LOG_DEBUG("#### passes ontology test\n");
    if (!validateInReplyTo(msg)) {LOG_DEBUG("\t\t\t\t*******9\n"); return false; }
    LOG_DEBUG("#### passes in reply to test\n");
    //if (!validateReplyBy(msg)) return false;
    
    
    return true;
     
}
void Transition::loadParameters()
{
    nextState = machine->getStateByName(nextStateName);
    updateRoles();
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
	      if (!it->role.compare(from) ) 
	      { 		
		expectedSenders.push_back(it->agent);
			
		if (it->agent == machine->owner) {  
					      removeAllAgentsBut(machine->owner,expectedSenders); //it = machine->involvedAgents.end();
					      break;
					   }
		else;
	      }else;
	  }
        } else;   
    } else throw std::runtime_error("Trying to use inexistent role name");
    
        
    if (machine->checkIfRoleExists(to) )
    {
         
        if (machine->checkIfRoleSet(to) )
        {
	 
	  std::vector<AgentMapping>::iterator it;
	  for (it = machine->involvedAgents.begin(); it != machine->involvedAgents.end(); it++)
	  {
	      if (!it->role.compare(to) ) 
	      {
		expectedRecepients.push_back(it->agent);
		if (it->agent == machine->owner) 
		{ 
		    removeAllAgentsBut(machine->owner,expectedRecepients); break; 
		} else;
	      } else;
	  }
        } else;
    } else throw std::runtime_error("Trying to use inexistent role name");
        
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
	  for (it = machine->involvedAgents.begin(); it != machine->involvedAgents.end(); it++)
	  {
	      if (!it->role.compare(from) ) 
	      {
		expectedSenders.push_back(it->agent);
		if (it->agent == machine->owner)
		{ 
		    removeAllAgentsBut(machine->owner,expectedSenders); 
		    break;
		     //it = machine->involvedAgents.end(); 
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
        return throw std::runtime_error("Trying to use inexistent role name");
    
    if (machine->checkIfRoleExists(to) )
    {
        if (machine->checkIfRoleSet(to) )
        {
	  std::vector<AgentMapping>::iterator it;
	  for (it = machine->involvedAgents.begin(); it != machine->involvedAgents.end(); it++)
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
        return throw std::runtime_error("Trying to use inexistent role name");
    
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
    LOG_DEBUG("\t# done with setting preceding states\n");
    owningState->resetInvolvedAgentsTicks();
    LOG_DEBUG("\t# done with reseting involved agents ticks\n");
    machine->currentState = nextState;
    
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

bool Transition::validateSender (ACLMessage &msg) const
{
    AgentID agent = msg.getSender();
    /*
    LOG_DEBUG("$$$$$$$$random test: expected senders of transition: ");
    for (std::vector<AgentID>::iterator it = expectedSenders.begin(); it != expectedSenders.end(); it++)
        LOG_DEBUG("%s", it->getName() );
    */    
    std::vector<AgentID>::iterator found = find(expectedSenders.begin(),expectedSenders.end(),agent);
    /*
    if (found != expectedSenders.end())
    LOG_DEBUG("\n\t$$$$$found sender: %s\n",found->getName() );
    else LOG_DEBUG("\n\t$$$$$found sender: NULL\n");
    */
    if ( found != expectedSenders.end() ) return true;
    return false;
}

bool Transition::validateRecepients (ACLMessage &msg) const
{
    LOG_DEBUG("\t#### call to calidate recepients\n");
    std::vector<AgentID> recepients = msg.getAllReceivers();
    std::vector<AgentID>::iterator it;
    
    /*
    LOG_DEBUG("^^^^ message from %s\t to %s ^^^^\n",from, to);
    std::vector<AgentID>::iterator test = expectedSenders.begin();
    std::vector<AgentID>::iterator test1 = expectedRecepients.begin();
    
    LOG_DEBUG("\t---expected senders are:\n");
    for (test; test != expectedSenders.end(); test++)
    {LOG_DEBUG("%s", test->getName() ); }
    LOG_DEBUG("\n\t---expected recepients are:\n");
    for (test1; test1 != expectedRecepients.end(); test1++)
    {LOG_DEBUG("%s", test1->getName() );}
    LOG_DEBUG("\n");
    
    LOG_DEBUG("\t###check###: \n");
    std::vector<AgentMapping>::iterator amp;
    for (amp = machine->involvedAgents.begin(); amp != machine->involvedAgents.end(); ++amp)
    {
        LOG_DEBUG( "#\t %s\t%s %d#\n", amp->role, amp->agent.getName(), amp->check);
    }
    */
    
    
    for (it = recepients.begin(); it != recepients.end(); it++)
    {
        
        LOG_DEBUG("^^^^ output from the actual check *** %s\t%s\n\n",expectedRecepients.begin()->getName(),it->getName() );
        std::vector<AgentID>::iterator found = find(expectedRecepients.begin(), expectedRecepients.end(),*it);
        if (found != expectedRecepients.end() ) ;
        else return false;
    }
    return true;
}

bool Transition::validateInReplyTo(ACLMessage &msg) const
{
    if (msg.getInReplyTo().empty()) return true;
    ACLMessage *fromArchive;
    std::vector<AgentID> recepients = msg.getAllReceivers();
    std::vector<AgentID>::iterator it;
    for (it = recepients.begin(); it != recepients.end(); it++)
    {
        if (precedingState == NULL) return true;
        if ( (fromArchive = precedingState->searchArchiveBySenderReceiver(msg.getSender(),*it)) == NULL ) return false;
        LOG_DEBUG("\t\t# not second return\n");
        //if ( (fromArchive->getReplyWith().empty() && (!msg.getInReplyTo().empty())) ||
	//   ((!fromArchive->getReplyWith().empty()) && msg.getInReplyTo().empty()) ) return false;
        if ( fromArchive->getReplyWith().compare(msg.getInReplyTo()) ) return false;
        
    }
    
    return true;
}

bool Transition::validatePerformative (ACLMessage &msg) const
{
    
    if (expectedPerf.compare(msg.getPerformative()) ) return false;
    return true;
}
bool Transition::validateOntology (ACLMessage &msg) const
{
    if (machine->ontology.empty() ) {machine->ontology = msg.getOntology(); return true;}
    if (machine->ontology.compare(msg.getOntology()) ) return false;
    return true;
}
bool Transition::validateEncoding (ACLMessage &msg) const
{
    if (machine->encoding.compare(msg.getEncoding()) ) return false;
    return true;
}
bool Transition::validateLanguage (ACLMessage &msg) const
{
    if (machine->language.empty() ) {machine->language = msg.getLanguage(); return true;}
    if (machine->language.compare(msg.getLanguage()) ) return false;
    return true;
}
bool Transition::validateProtocol (ACLMessage &msg) const
{
    if (machine->protocol.empty() ) {machine->protocol = msg.getProtocol(); return true;}
    if (machine->protocol.compare(msg.getProtocol()) ) return false;
    return true;
}
bool Transition::validateConvID (ACLMessage &msg) const
{
    if (machine->convid.empty() ) {machine->convid = msg.getConversationID(); return true;}
    if (machine->convid.compare(msg.getConversationID()) ) return false;
    return true;
}

void Transition::removeAllAgentsBut(const AgentID &ag,std::vector<AgentID> &agents)
{
    LOG_DEBUG("RemoveAllAgentsBut: %s\n", ag.getName() );
    agents.clear();
    agents.push_back(ag);
    LOG_DEBUG("RemoveAllAgentsBut: end %s\n", ag.getName() );
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