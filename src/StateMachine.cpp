/*
big TODO(for me - not relevant right now but for the general case): line 203(behaviour in case a conversation is successfully cancelled);

note for future tweak: make it possible that once a machine has been built, if we try to start it with a message and it fails,
		   make it possible to be able to attempt to start it again during runtime(the same machine - currently it needs rebuilding)

note for future: the "in reply to" policy needs to be defined somehow (not necessarily important for this stage); we need to define 
	       in which contexts the "in reply to" field must be filled in in order to correctly validate a message; 
	       
	       current policy is: if the field is not filled in then it means it should not be(thus considerred valid)
	       
	       another intuitive policy might be(not yet sure whether it stands for the general case): check all the messages 
	       from preceding state and if there if there is no message TO the current sender with a "in reply to" field filled in, 
	       then it should not be filled in
*/
#include "StateMachine.h"
#include "State.h"

#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <base/logging.h>

namespace fipa {
namespace acl {

const std::string StateMachine::NOT_UNDERSTOOD = std::string("not-understood");
const std::string StateMachine::CONV_CANCELLED = std::string("conv-cancelled");
const std::string StateMachine::INITIAL = std::string("initial");
const std::string StateMachine::INITIATOR = std::string("initiator");
void StateMachine::initializeObjectFields()
{
    
    
    involvedAgents.clear();
    preImposedRoles.clear();
    states.clear();
    currentState = NULL;
    owner = AgentID();
    conversationOver = false;
    active = false;
    cancelMetaP.clear();
    
    language.clear();
    ontology.clear();
    protocol.clear();
    convid.clear();
    encoding.clear();
    

}
bool StateMachine::setInitialState(State *_state)
{
    if (active) return false;
    currentState = _state;
    return true;
}

bool StateMachine::setInitialState(const std::string _uid)
{
    if (active) return false;
    State *s = getStateByName(_uid);
    if (!s) return false;
    currentState = s;
    return true;
}

bool StateMachine::setOwner(const AgentID& _owner)
{
    if (active) return false;
    owner = _owner;
    return true;
}

void StateMachine::generateDefaultStates()
{
    //currentState = NULL;
    /*
    State initial = State(StateMachine::INITIAL);
    initial.setFinal(false);
    states.push_back(initial);
    currentState = &(*find(states.begin(),states.end(),StateMachine::INITIAL));
    */
    State notUnderstood = State(StateMachine::NOT_UNDERSTOOD);
    notUnderstood.setFinal(true);
    
    //State convCancelled = State(StateMachine::CONV_CANCELLED);
    //convCancelled.setFinal(true);
    
    states.push_back(notUnderstood);
    //states.push_back(convCancelled);
}

void StateMachine::generateDefaultTransitions()
{
    std::vector<State>::iterator it;
    for (it = states.begin(); it != states.end(); it++)
    {
        if (!it->getUID().compare(StateMachine::NOT_UNDERSTOOD)) continue;
        //if (!it->getUid().compare(StateMachine::CONV_CANCELLED)) continue;
        //if (!it->getUid().compare(StateMachine::INITIAL) continue;
        
        it->generateDefaultTransitions();        
    }
}
//&&&&&&&&&&&&&NOTE:remember to treat not understood transitions differently when executing as well as when loading--not so sure i still need to(on exec)&&&&&&&&&&//
//~~~~&&&&&&&&&&&&&NOTE:remember to check for duplicates when adding a transition to a state&&&&&&&&&&//

int StateMachine::createAndStartNewCancelMetaProtocol(const ACLMessage &msg)
{
    std::vector<AgentID> interlocutors = msg.getAllReceivers();
    for (std::vector<AgentID>::iterator it = interlocutors.begin(); it != interlocutors.end(); it++)
    {
        cancelMetaP.push_back(generateCancelMetaProtocol(std::string("with")));
        if (cancelMetaP.back().startMachine(msg)) return 1;
    }
    return 0;
}

StateMachine StateMachine::generateCancelMetaProtocol(Role with)
{
    //std::vector<AgentMapping>::iterator it;
    //for (it = involvedAgents.begin(); it != involvedAgents.end(); it++)
   // {
        //if (!it->Role.compare(StateMachine::INITIATOR)) continue;
        StateMachine temp = StateMachine();
        temp.addRole(StateMachine::INITIATOR);
        temp.addRole(with);
        
        State initial = State(StateMachine::INITIAL);
        initial.setFinal(false);
        states.push_back(initial);
        currentState = &(*find(states.begin(),states.end(),StateMachine::INITIAL));
        temp.generateDefaultStates();
        
        State first = State(std::string("1"));
        first.setFinal(false);
        State second = State(std::string("2"));
        second.setFinal(true);
        
        temp.addState(first);
        temp.addState(second);
        
        Transition t1 = Transition();
        t1.setExpectedPerformative(ACLMessage::perfs[ACLMessage::CANCEL]);
        t1.setFrom(StateMachine::INITIATOR);
        t1.setTo(with);
        t1.setNextStateName(std::string("1"));
        //t1.setMessageParity(true);
        temp.getStateByName(StateMachine::INITIAL)->addTransition(t1);
        
        Transition t2 = Transition();
        t2.setExpectedPerformative(ACLMessage::perfs[ACLMessage::INFORM]);
        t2.setFrom(with);
        t2.setTo(StateMachine::INITIATOR);
        t2.setNextStateName(std::string("2"));
        //t2.setMessageParity(true);
        temp.getStateByName(std::string("1"))->addTransition(t2);
        
        t2.setExpectedPerformative(ACLMessage::perfs[ACLMessage::FAILURE]);
        t2.setFrom(with);
        t2.setTo(StateMachine::INITIATOR);
        t2.setNextStateName(std::string("2"));
        //t2.setMessageParity(true);
        temp.getStateByName(std::string("1"))->addTransition(t2);
        
        temp.generateDefaultTransitions();
        //cancelMetaP.push_back(temp);
        
    //}
    return temp;
}

StateMachine::StateMachine()
{
    initializeObjectFields();
}
StateMachine::StateMachine(const AgentID& _owner)
{
    initializeObjectFields();
    owner = _owner;
    
}

StateMachine::StateMachine(const StateMachine& target)
{
    std::vector<State> tempStates = target.getStates();
    for (std::vector<State>::iterator it = tempStates.begin(); it != tempStates.end(); it ++)
        addState(*it);
    
    involvedAgents = target.getInvolvedAgents();
    owner = target.getOwner();
    
    currentState = getStateByName(target.getCurrentState()->getUID());
    
    active = target.isActive();
    conversationOver = target.isConversationOver();
    
    cancelMetaP = target.getCancelMetaP();
    
    ontology = target.getOntology();
    language = target.getLanguage();
    encoding = target.getEncoding();
    protocol = target.getProtocol();
    convid = target.getConversationID();
        
}

StateMachine::~StateMachine()
{
    states.clear();
    involvedAgents.clear();
    cancelMetaP.clear();
}

int StateMachine::startMachine(const ACLMessage& msg)
{
    int x;
    if (active) return 1;
    active = true;
    convid = msg.getConversationID();
    protocol = msg.getProtocol();
    encoding = msg.getEncoding();
    language = msg.getLanguage();
    ontology = msg.getOntology();
    loadParameters();
    if ((x = consumeMessage(msg)) != 0) return x;

    
    return 0;
    
}

int StateMachine::consumeMessage(const ACLMessage& msg)
{
    LOG_DEBUG("enter consume..\n");
    if (!active) return startMachine(msg);
    int x = 0; // return code received from the current state
    std::vector<StateMachine>::iterator it;
    for (it = cancelMetaP.begin(); it != cancelMetaP.end(); it++)
    {
        if (it->isActive() )
        {	  
	  LOG_DEBUG("meta conversation active..\n");
	  if (it->consumeMessage(msg) == 0) return 0;
	  //x= 1;
        }
    }
    if (!msg.getPerformative().compare(ACLMessage::perfs[ACLMessage::CANCEL]) )
        if (!getAgentRole(msg.getSender()).compare(StateMachine::INITIATOR) )
	  return createAndStartNewCancelMetaProtocol(msg);
        else return 1;
    else;
    
    LOG_DEBUG("sending the message to the current state..\n");
    if (currentState)
    {
        LOG_DEBUG("current state not-null..\n");
        if (currentState->consumeMessage(msg) == 0)   
        {
	  if (currentState->getFinal() ) { conversationOver = true; active = false; return 0;}
	  else return 0;
        }
    }else; //TODO: twrow some error here
    return 1;
}

void StateMachine::updateAllAgentRoles()
{
    std::vector<State>::iterator it;
    for (it = states.begin(); it != states.end(); it++)
    {
        it->updateAllAgentRoles();
    }
}

void StateMachine::updateAllAgentRoles(const Role& myrole, const std::vector<AgentID>& myagents)
{
    std::vector<RoleCorrelation>::iterator it;
    for(it = preImposedRoles.begin(); it != preImposedRoles.end(); it++)
    {
        if (!it->master.compare(myrole) && !it->check)
        {
	  setRole(it->resident, myagents);
	  it->check = true;
	  break;
        }
    }
    updateAllAgentRoles();
}

State* StateMachine::getStateByName(const std::string& _name)
{
    std::vector<State>::iterator it = find(states.begin(),states.end(),_name);
    if (it == states.end()) return NULL;
    return (&(*it));
    
    /*for (it = states.begin(); it != states.end(); it ++)
    {
        if (!it->getUID().compare(_name)) return (&(*it));
    }
    return NULL;*/
}

State* StateMachine::getStateByName(State _name) const
{
    std::string searchName = _name.getUID();
    std::vector<State>::iterator it = find(states.begin(),states.end(),searchName);
    if (it == states.end()) return NULL;
    return (&(*it));
}
void StateMachine::removeInterlocutor(const AgentID& ag)
{
    std::vector<AgentMapping>::iterator it;
    for (it = involvedAgents.begin(); it != involvedAgents.end(); it++)
    {
        if (it->agent == ag)
	  if (it->check) { involvedAgents.erase(it); break; }
    }
}
void StateMachine::removeInterlocutor(const std::vector<AgentID>& agents)
{
    std::vector<AgentID>::iterator it;
    for (it = agents.begin(); it != agents.end(); it++)
        removeInterlocutor(*it);
}

bool StateMachine::setRole(const Role& myrole,const AgentID& myagent)
{
    std::vector<AgentMapping>::iterator it;
    if (checkIfAgentAssigned(myagent) ) return false;
    for (it = involvedAgents.begin(); it != involvedAgents.end(); it++)
    {
        if (!it->role.compare(myrole) )
	  if (!it->check) 
	  {
	      it->agent = myagent;
	      it->check = true;
	      LOG_DEBUG("\t^^^^set the role %s to agent %s ^^^^\n", myrole, myagent.getName());
	      return true;
	  }
    }
    return false;
}
bool StateMachine::setRole(const Role& myrole, const std::vector<AgentID>& agents)
{
    //if (checkIfRoleSet(myrole) ) return false;
    if (agents.empty()) return true;
   
    std::vector<AgentID>::iterator agbegin = agents.begin();
    std::vector<AgentID>::iterator agend = agents.end();
    std::vector<AgentID>::iterator agit;
    /*for (agit = agbegin; agit != agend; agit++)
    {
        LOG_DEBUG("**checking if agent to be set isn't already set\t%s\t%s\n", agit->getName(), (agents.begin())->getName() );
        if (checkIfAgentAssigned(*agit) ) return false;
    }*/
    
    
    std::vector<AgentMapping>::iterator aux;
    bool done = false;
    while(!done)
    {
     std::vector<AgentMapping>::iterator it;
     done = true;
     for (it = involvedAgents.begin(); it != involvedAgents.end(); it++)
        if (!it->role.compare(myrole) )
	  if (!it->check) 
	  {	      
	      involvedAgents.erase(it);
	      done = false;
	      break;
	  }
    }
	
    for (agit = agbegin; agit != agend; agit++)
    {
        AgentMapping element;
        element.role = myrole;
        element.agent = *agit;
        LOG_DEBUG("\t^^^^set the role %s to agent %s ^^^^\n", myrole, agit->getName() );
        element.check = true;
        LOG_DEBUG("\tbuilt a new element\n");
        involvedAgents.push_back(element);
        LOG_DEBUG("\t**pushed a new agent\n");
    }
    return true;
}

bool StateMachine::checkIfAgentAssigned(AgentID& ag) const
{
    std::vector<AgentMapping>::iterator it;
    for (it = involvedAgents.begin(); it != involvedAgents.end(); it++)
    {
        if (it->agent == ag)
	  if (it->check == true) return true;
    }
    return false;
}

std::vector<AgentID> StateMachine::getAgentsAssignedTo(Role& myrole) const
{
    std::vector<AgentMapping>::iterator it;
    std::vector<AgentID> ret; ret.clear();
    for (it = involvedAgents.begin(); it != involvedAgents.end(); it++)
    {
        if (!it->role.compare(myrole) && it->check)
        {
	  ret.push_back(it->agent);
        }
    }
    return ret;
}

bool StateMachine::checkIfRoleSet(Role &myrole) const
{
    std::vector<AgentMapping>::iterator it;
    for (it = involvedAgents.begin(); it != involvedAgents.end(); it++)
    {
        if (!it->role.compare(myrole) )
	  return it->check;
    }
    return false;
}

bool StateMachine::isConversationOver() const
{
    return conversationOver;
}
bool StateMachine::isActive() const
{
    return active;
}
bool StateMachine::checkIfRoleExists(Role& myrole) const
{
    //std::cout<<"checking for role:\t"<<myrole<<"\n";
   
    std::vector<AgentMapping>::iterator it;
    for (it = involvedAgents.begin(); it != involvedAgents.end(); it++)
    {
       if (!(it->role.compare(myrole)) ) return true;
    }
    return false;
}

bool StateMachine::addState(const State& _state)
{
    std::string name = _state.getUID();
    //if (_state.getOwningMachine() == NULL)
        _state.setOwningMachine(this);
    if (find(states.begin(),states.end(),name) == states.end() ) {states.push_back(_state); return true;}
    return false;
}

Role StateMachine::getAgentRole(AgentID& ag) const
{
    std::vector<AgentMapping>::iterator it;
    for (it = involvedAgents.begin(); it != involvedAgents.end(); it++)
        if ((it->agent == ag) && (it->check == true)) return it->role;
        return std::string("");
}

void StateMachine::addRoleCorrelation(const Role& mymaster, const Role& myresident)
{    
    if (!checkIfRoleExists(myresident)) 
        return;
    
    for (std::vector<RoleCorrelation>::iterator it = preImposedRoles.begin(); it != preImposedRoles.end(); it++)
        if (!it->master.compare(mymaster) && !it->resident.compare(myresident))
	  return;
    
    RoleCorrelation temp;
    temp.master = mymaster;
    temp.resident = myresident;
    temp.check = false;
    
    preImposedRoles.push_back(temp);
}

std::vector<RoleCorrelation> StateMachine::getRoleCorrelation() const
{
    return preImposedRoles;
}

void StateMachine::addRole(const Role& myrole)
{
    if (checkIfRoleExists(myrole) ) return; //either this or throw some roleAlreadyExists error
    AgentMapping newRole;
    newRole.role = myrole;
    newRole.check = false;
    involvedAgents.push_back(newRole);
    

}

void StateMachine::loadParameters()
{
    std::vector<State>::iterator sit;
    for (sit = states.begin(); sit != states.end(); ++sit)
    {
        sit->loadParameters();
    }
}

Language 			StateMachine::getLanguage() const		{return language;}
Ontology 			StateMachine::getOntology() const		{return ontology;}
Encoding 			StateMachine::getEncoding() const		{return encoding;}
Protocol 			StateMachine::getProtocol() const		{return protocol;}
std::vector<StateMachine> 	StateMachine::getCancelMetaP() const		{return cancelMetaP;}
State* 			StateMachine::getCurrentState() const		{return currentState;}
std::vector<AgentMapping> 	StateMachine::getInvolvedAgents () const	{return involvedAgents;}
AgentID 			StateMachine::getOwner() const		{return owner;}
std::vector<State> 		StateMachine::getStates() const		{return states;}
ConversationID		StateMachine::getConversationID() const		{return convid;}

void StateMachine::print()
{
    std::cout<<"========================"<<" StateMachine "<<"========================\n";
    std::cout<<"involvedAgents: ";
    for (std::vector<AgentMapping>::iterator it = involvedAgents.begin(); it != involvedAgents.end(); it++)
        std::cout<< it->role <<" ";
    
    std::cout<<"\ncurrentState: ";
    if (currentState)
        std::cout<< currentState->getUID() <<"\n";
    else
        std::cout<<"currentState not set\n";
    
    std::cout<<"owner: " << owner.getName() <<"\n";
    std::cout<<"active: "<<active<<"\n";
    std::cout<<"conversationOver: "<<conversationOver<<"\n";
    std::cout<<"ontology: "<<ontology<<"\n";
    std::cout<<"protocol: "<<protocol<<"\n";
    std::cout<<"language: "<<language<<"\n";
    std::cout<<"conversation ID: "<<convid<<"\n";
    std::cout<<"encoding: "<<encoding<<"\n\n";
    std::cout<<"********** States: **********\n";
    
    for (std::vector<State>::iterator it = states.begin(); it != states.end(); it++)
        it->print();
    
    std::cout<<"========================"<<"=============="<<"========================\n";
}



} // end of acl
} // end of fipa
