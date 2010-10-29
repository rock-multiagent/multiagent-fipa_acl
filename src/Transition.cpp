
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
    std::cout<<"\towningState is:\t\t"<< owningState->getUID() <<"\n";
    if (validateMessage(msg))
    {
        //if (expectedPerf.compare(ACLMessage::perfs[ACLMessage::NOT_UNDERSTOOD])) return processNotUnderstood(msg);
        std::cout<< "#### validated message\n";
        performWithoutStateExit(msg);
        //if ( checkAllSendersAccountedFor(msg) && checkAllRecepientsAccountedFor(msg) )
        std::cout<< "#### done with perform without state exit\n";
        if (owningState->checkAllAgentsAccountedFor() )
        {
	  std::cout<< "#### all agents accounted for\n";
	  performOnStateExit(msg);
	  std::cout<< "#### done with perform on state exit\n";
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

bool Transition::validateMessage(ACLMessage &msg)
{
    if (!validatePerformative(msg)){std::cout<<"\t\t\t\t*******1\n"; return false; }
    if (expectedSenders.empty() || expectedRecepients.empty() ) 
    {std::cout<<"agents not set yet..\n"; if (!updateRoles(msg)) 
        { std::cout<<"updateRoles(msg) returned 0\n"; return false;} std::cout<<"agents set now..\n";}
        
    if (!validateSender(msg)) {std::cout<<"\t\t\t\t*******2\n"; return false; }
    if (!validateRecepients(msg)) {std::cout<<"\t\t\t\t*******3\n"; return false; }
    
    if (!validateConvID(msg)) {std::cout<<"\t\t\t\t*******4\n"; return false; }
    if (!validateProtocol(msg)) {std::cout<<"\t\t\t\t*******5\n"; return false; }
    if (!validateEncoding(msg)) {std::cout<<"\t\t\t\t*******6\n"; return false; }
    if (!validateLanguage(msg)) {std::cout<<"\t\t\t\t*******7\n"; return false; }
    std::cout<< "#### passes language test\n";
    if (!validateOntology(msg)) {std::cout<<"\t\t\t\t*******8\n"; return false; }
    std::cout<< "#### passes ontology test\n";
    if (!validateInReplyTo(msg)) {std::cout<<"\t\t\t\t*******9\n"; return false; }
    std::cout<< "#### passes in reply to test\n";
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
		if (it->agent == machine->owner) { removeAllAgentsBut(machine->owner,expectedSenders); //it = machine->involvedAgents.end(); 
					  break;
					  }
		else;
	      }
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
	      if (!it->role.compare(to) ) 
	      {
		expectedRecepients.push_back(it->agent);
		if (it->agent == machine->owner) { removeAllAgentsBut(machine->owner,expectedRecepients); break; }
		else;
	      }
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
       
        if(machine->checkIfRoleSet(from) )
        {
	  
	  std::vector<AgentMapping>::iterator it;
	  for (it = machine->involvedAgents.begin(); it != machine->involvedAgents.end(); it++)
	  {
	      if (!it->role.compare(from) ) 
	      {
		expectedSenders.push_back(it->agent);
		if (it->agent == machine->owner) { removeAllAgentsBut(machine->owner,expectedSenders); 
					      break;
					      //it = machine->involvedAgents.end(); 
					   }
		else;
	      }
	  }
        } else 
	  {
	      
	      if (!machine->setRole(from,msg.getSender())) return false;
	      machine->updateAllAgentRoles();
	      int myc = 0;
	      std::vector<State>::iterator trcount;
    for (trcount = (machine->states).begin(); trcount != (machine->states).end(); trcount++)
    {
        myc += trcount->transitions.size(); 
    }
    std::cout<<"&&&&&&&there are currently "<<myc<<" transitions in the SM\n";
    std::cout<<"&&&&&&&there are currently "<< machine->states.size() <<" states in the SM\n";
	  }
        
    } else return false; //TODO: twrow some violation of protocol error
    
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
		
	      if (!(machine->setRole(to,msg.getAllReceivers())) ) return false;
	      machine->updateAllAgentRoles();
	      
	  }
    } else return false; //TODO: throw some violation of protocol error
        std::cout<<"!!!exit update transition with message as param%%%%%%%%\n";
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
    if (nextState == NULL) std::cout<< "\t# next state is NULL\n";
    nextState->setAllPrecedingStates(owningState);
    std::cout<< "\t# done with setting preceding states\n";
    owningState->resetInvolvedAgentsTicks();
    std::cout<< "\t# done with reseting involved agents ticks\n";
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

bool Transition::validateSender (ACLMessage &msg)
{
    AgentID agent = msg.getSender();
    std::vector<AgentID>::iterator found = find(expectedSenders.begin(),expectedSenders.end(),agent);
    if ( found != expectedSenders.end() ) return true;
    return false;
}

bool Transition::validateRecepients (ACLMessage &msg)
{
    std::cout<< "\t#### call to calidate recepients\n";
    std::vector<AgentID> recepients = msg.getAllReceivers();
    std::vector<AgentID>::iterator it;
    
    /*
    std::cout<<"^^^^ message from "<< from<<"\t to "<<to<<"^^^^\n";
    std::vector<AgentID>::iterator test = expectedSenders.begin();
    std::vector<AgentID>::iterator test1 = expectedRecepients.begin();
    
    std::cout<<"\t---expected senders are:\n";
    for (test; test != expectedSenders.end(); test++)
    {std::cout << test->getName()<<" "; }
    std::cout<<"\n\t---expected recepients are:\n";
    for (test1; test1 != expectedRecepients.end(); test1++)
    {std::cout << test1->getName()<<" ";}
    std::cout<<"\n";
    
    std::cout << "\t###check###: \n";
    std::vector<AgentMapping>::iterator amp;
    for (amp = machine->involvedAgents.begin(); amp != machine->involvedAgents.end(); ++amp)
    {
        std::cout << "#\t" << amp->role << "\t" << amp->agent.getName() << " " << amp->check << "#\n";
    }
    */
    
    
    for (it = recepients.begin(); it != recepients.end(); it++)
    {
        
        std::cout<< "^^^^ output from the actual check *** "<< expectedRecepients.begin()->getName() <<"\t"<< it->getName() <<"\n\n";
        std::vector<AgentID>::iterator found = find(expectedRecepients.begin(), expectedRecepients.end(),*it);
        if (found != expectedRecepients.end() ) ;
        else return false;
    }
    return true;
}

bool Transition::validateInReplyTo(ACLMessage &msg)
{
    if (msg.getInReplyTo().empty()) return true;
    ACLMessage *fromArchive;
    std::vector<AgentID> recepients = msg.getAllReceivers();
    std::vector<AgentID>::iterator it;
    for (it = recepients.begin(); it != recepients.end(); it++)
    {
        if (precedingState == NULL) return true; std::cout<< "\t\t# not first return\n";
        if ( (fromArchive = precedingState->searchArchiveBySenderReceiver(msg.getSender(),*it)) == NULL ) return false;
        std::cout<< "\t\t# not second return\n";
        //if ( (fromArchive->getReplyWith().empty() && (!msg.getInReplyTo().empty())) ||
	//   ((!fromArchive->getReplyWith().empty()) && msg.getInReplyTo().empty()) ) return false;
        if ( fromArchive->getReplyWith().compare(msg.getInReplyTo()) ) return false;
        std::cout<< "\t\t# not third return\n";
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
    if (machine->language.empty() ) {machine->language = msg.getLanguage(); return true;}
    if (machine->language.compare(msg.getLanguage()) ) return false;
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

void Transition::removeAllAgentsBut(AgentID &ag,std::vector<AgentID> &agents)
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
void Transition::setMachine		(StateMachine* _machine)	{machine = _machine;}
    
std::string Transition::getExpectedPerformative()	{return expectedPerf; }
std::string Transition::getNextStateName()	{return nextStateName; }
std::string Transition::getFrom()		{return from; }
std::string Transition::getTo()		{return to; }
State* Transition::getNextState()		{return nextState; }
std::vector<AgentID> Transition::getExpectedSenders() 	{return expectedSenders; }
std::vector<AgentID> Transition::getExpectedRecepients()	{return expectedRecepients; }
StateMachine* Transition::getMachine()			{return machine;}



} // end of acl
} // end of fipa