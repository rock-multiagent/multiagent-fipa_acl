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
#include "statemachine.h"
#include "state.h"

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

bool StateMachine::setInitialState(const std::string& stateid)
{
    if (active) return false;
    currentState = stateid;
    return true;
}

bool StateMachine::setOwner(const AgentID& _owner)
{
    owner = _owner;
    return true;
}

void StateMachine::generateDefaultStates()
{
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
    for (it = states.begin(); it != states.end(); ++it)
    {
        //if (!it->getUid().compare(StateMachine::CONV_CANCELLED)) continue;
        //if (!it->getUid().compare(StateMachine::INITIAL) continue;
        if(it->isFinal())
            continue;

#warning "Default transitions not enabled"
    }
}
//NOTE:remember to treat not understood transitions differently when executing as well as when loading--not so sure i still need to(on exec)
//NOTE:remember to check for duplicates when adding a transition to a state

bool StateMachine::createAndStartNewCancelMetaProtocol(const ACLMessage &msg)
{
    std::vector<AgentID> interlocutors = msg.getAllReceivers();
    for (std::vector<AgentID>::iterator it = interlocutors.begin(); it != interlocutors.end(); ++it)
    {
        cancelMetaP.push_back(generateCancelMetaProtocol(std::string("with")));
        return cancelMetaP.back().consumeMessage(msg);
    }
    return false;
}

StateMachine StateMachine::generateCancelMetaProtocol(Role with)
{
    //std::vector<AgentMapping>::iterator it;
    //for (it = involvedAgents.begin(); it != involvedAgents.end(); ++it)
   // {
        //if (!it->Role.compare(StateMachine::INITIATOR)) continue;
        StateMachine temp = StateMachine();
        temp.addRole(StateMachine::INITIATOR);
        temp.addRole(with);
        
        State initial = State(StateMachine::INITIAL);
        initial.setFinal(false);
        states.push_back(initial);
        std::vector<State>::iterator sit = states.begin();
        currentState = StateMachine::INITIAL;
        temp.generateDefaultStates();
        
        State first = State(std::string("1"));
        first.setFinal(false);
        State second = State(std::string("2"));
        second.setFinal(true);
        
        temp.addState(first);
        temp.addState(second);
        
        Transition t1 = Transition();
        t1.setExpectedPerformative(PerformativeTxt[ACLMessage::CANCEL]);
        t1.setFrom(StateMachine::INITIATOR);
        t1.setTo(with);
        t1.setNextStateName(std::string("1"));
        //t1.setMessageParity(true);
        temp.getStateByName(StateMachine::INITIAL)->addTransition(t1);
        
        Transition t2 = Transition();
        t2.setExpectedPerformative(PerformativeTxt[ACLMessage::INFORM]);
        t2.setFrom(with);
        t2.setTo(StateMachine::INITIATOR);
        t2.setNextStateName(std::string("2"));
        //t2.setMessageParity(true);
        temp.getStateByName(std::string("1"))->addTransition(t2);
        
        t2.setExpectedPerformative(PerformativeTxt[ACLMessage::FAILURE]);
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
    : active(false)
    , conversationOver(false)
    , isValidStateMachine(false)
{
}

StateMachine::StateMachine(const AgentID& _owner)
    : active(false)
    , conversationOver(false)
    , isValidStateMachine(false)
{
    owner = _owner;
}

StateMachine::StateMachine(const StateMachine& target) 
{
    states = target.states;
    involvedAgents = target.involvedAgents;
    owner = target.owner;
    currentState = target.currentState; 

    active = target.isActive();
    conversationOver = target.isConversationOver();
    
    cancelMetaP = target.getCancelMetaP();
    
    ontology = target.getOntology();
    language = target.getLanguage();
    encoding = target.getEncoding();
    protocol = target.getProtocol();
    convid = target.getConversationID();
    isValidStateMachine = target.isValid();

    std::vector<State>::iterator it = states.begin();

    for(; it != states.end(); ++it)
    {
        it->setOwningMachine(this);
    }
}

StateMachine& StateMachine::operator=(const StateMachine& target)
{
    if(this != &target)
    {
        states = target.states;
        involvedAgents = target.involvedAgents;
        owner = target.owner;
        currentState = target.currentState; 

        active = target.isActive();
        conversationOver = target.isConversationOver();
        
        cancelMetaP = target.getCancelMetaP();
        
        ontology = target.getOntology();
        language = target.getLanguage();
        encoding = target.getEncoding();
        protocol = target.getProtocol();
        convid = target.getConversationID();
        isValidStateMachine = target.isValid();

        std::vector<State>::iterator it = states.begin();
        for(; it != states.end(); ++it)
        {
            it->setOwningMachine(this);
        }
    }
    return *this;
}

StateMachine::~StateMachine()
{
    states.clear();
    involvedAgents.clear();
    cancelMetaP.clear();
}

void StateMachine::startMachine(const ACLMessage& msg)
{
    LOG_DEBUG("Start machine for owner: %s", owner.getName().c_str());
    active = true;
    convid = msg.getConversationID();
    protocol = msg.getProtocol();
    encoding = msg.getEncoding();
    language = msg.getLanguage();
    ontology = msg.getOntology();
    loadParameters();
}

bool StateMachine::consumeMessage(const ACLMessage& msg)
{
    LOG_DEBUG("Enter consume");
    if(!active)
    {
        startMachine(msg);
    }

    std::vector<StateMachine>::iterator it;
    for (it = cancelMetaP.begin(); it != cancelMetaP.end(); ++it)
    {
        if (it->isActive() )
        {	  
	    LOG_DEBUG("meta conversation active");
	    if (it->consumeMessage(msg))
            { 
                return true;
            }    
        }
    }

    if( msg.getPerformative() == PerformativeTxt[ACLMessage::CANCEL] )
    {
        if ( getAgentRole(msg.getSender()) == StateMachine::INITIATOR )
        {
	    return createAndStartNewCancelMetaProtocol(msg);
        } else {
            return false;
        }
    }
    
    State* state = getCurrentState();
    assert(state);
    LOG_DEBUG("sending the message %s to the current state: %s", msg.getPerformative().c_str(), state->getUID().c_str());

    if(state->consumeMessage(msg))   
    {
          state = getCurrentState();
          LOG_DEBUG("State after consuming msg %s state: %s", msg.getPerformative().c_str(), state->getUID().c_str());
          if (state->isFinal() ) 
          { 
              conversationOver = true; 
              active = false; 
          }
          return true;
    } else {
        return false;
    }
}

void StateMachine::updateAllAgentRoles()
{
    std::vector<State>::iterator it;
    for (it = states.begin(); it != states.end(); ++it)
    {
        it->updateAllAgentRoles();
    }
}

void StateMachine::updateAllAgentRoles(const Role& myrole, const std::vector<AgentID>& myagents)
{
    std::vector<RoleCorrelation>::iterator it;
    for(it = preImposedRoles.begin(); it != preImposedRoles.end(); ++it)
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
    if (it == states.end()) 
    {
        LOG_WARN("State not found: %s in machine %p of owner: %s", _name.c_str(), this, owner.getName().c_str());
        return NULL;
    }
    return (&(*it));
}

State* StateMachine::getStateByName(State _name) const
{
    std::string searchName = _name.getUID();
    std::vector<State>::const_iterator it = states.begin();
    for( it = states.begin(); it != states.end(); ++it)
    {
        if(it->getUID() == searchName)
            return const_cast<State*>(&(*it));
    }
    return NULL;
}
void StateMachine::removeInterlocutor(const AgentID& ag)
{
    std::vector<AgentMapping>::iterator it;
    for (it = involvedAgents.begin(); it != involvedAgents.end(); ++it)
    {
        if (it->agent == ag)
	  if (it->check) { involvedAgents.erase(it); break; }
    }
}
void StateMachine::removeInterlocutor(const std::vector<AgentID>& agents)
{
    std::vector<AgentID>::const_iterator it;
    for (it = agents.begin(); it != agents.end(); ++it)
        removeInterlocutor(*it);
}

bool StateMachine::setRole(const Role& myrole,const AgentID& myagent)
{
    std::vector<AgentMapping>::iterator it;
    if (checkIfAgentAssigned(myagent) ) return false;
    for (it = involvedAgents.begin(); it != involvedAgents.end(); ++it)
    {
        if (it->role == myrole && !it->check) 
	{
	      it->agent = myagent;
	      it->check = true;
	      LOG_DEBUG("Assign role %s to agent %s", myrole.c_str(), myagent.getName().c_str());
	      return true;
	}
    }
    return false;
}
bool StateMachine::setRole(const Role& myrole, const std::vector<AgentID>& agents)
{
    if (agents.empty()) 
        return true;
  
    // Find the mapping that is still open 
    // and replace it by the newly assigned agents
    std::vector<AgentMapping>::iterator it;
    bool found = false;
    for (it = involvedAgents.begin(); it != involvedAgents.end(); ++it)
    {
       if (it->role == myrole && !it->check)
       {
         involvedAgents.erase(it);
         found = true;
         break;
       }
    }

    if(!found)
    {
        LOG_ERROR("Agent role could not be assigned");
        return false;
    }

    // Assign role to agents 
    std::vector<AgentID>::const_iterator agit = agents.begin();
    for (; agit != agents.end();++agit)
    {
        AgentMapping element;
        element.role = myrole;
        element.agent = *agit;
        element.check = true;
        involvedAgents.push_back(element);

        LOG_DEBUG("Assign role %s to agent %s", myrole.c_str(), agit->getName().c_str() );
    }
    return true;
}

bool StateMachine::checkIfAgentAssigned(const AgentID& ag)
{
    std::vector<AgentMapping>::iterator it;
    for (it = involvedAgents.begin(); it != involvedAgents.end(); ++it)
    {
        if (it->agent == ag && it->check == true)
           return true;
    }
    return false;
}

std::vector<AgentID> StateMachine::getAgentsAssignedTo(Role& myrole) const
{
    std::vector<AgentMapping>::const_iterator it = involvedAgents.begin();
    std::vector<AgentID> ret; 
    for(; it != involvedAgents.end(); ++it)
    {
        if (it->role == myrole && it->check)
        {
	  ret.push_back(it->agent);
        }
    }
    return ret;
}

bool StateMachine::checkIfRoleSet(const Role &myrole)
{
    std::vector<AgentMapping>::iterator it;
    for (it = involvedAgents.begin(); it != involvedAgents.end(); ++it)
    {
        if (it->role == myrole)
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
bool StateMachine::checkIfRoleExists(const Role& myrole)
{
    std::vector<AgentMapping>::iterator it;
    for (it = involvedAgents.begin(); it != involvedAgents.end(); ++it)
    {
       if (it->role == myrole)
           return true;
    }
    return false;
}

bool StateMachine::addState(State& _state)
{
    std::string name = _state.getUID();
    _state.setOwningMachine(this);
    if ( std::find(states.begin(),states.end(),name) == states.end() )
    {
        states.push_back(_state);
        return true;
    }
    return false;
}

Role StateMachine::getAgentRole(const AgentID& ag)
{
    std::vector<AgentMapping>::const_iterator it;
    for (it = involvedAgents.begin(); it != involvedAgents.end(); ++it)
    {
        if ((it->agent == ag) && (it->check == true))
            return it->role;
    }
    return std::string("");
}

void StateMachine::addRoleCorrelation(const Role& mymaster, const Role& myresident)
{    
    if (!checkIfRoleExists(myresident)) 
        return;
    
    for (std::vector<RoleCorrelation>::iterator it = preImposedRoles.begin(); it != preImposedRoles.end(); ++it)
        if (it->master == mymaster && it->resident == myresident)
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
    if (checkIfRoleExists(myrole) ) 
        return; //either this or throw some roleAlreadyExists error
    AgentMapping newRole;
    newRole.role = myrole;
    newRole.check = false;
    involvedAgents.push_back(newRole);
}

void StateMachine::loadParameters()
{
    LOG_INFO("StateMachine # of state is: %d", states.size());
    std::vector<State>::iterator sit;
    for (sit = states.begin(); sit != states.end(); ++sit)
    {
        assert(&(*sit));
        sit->loadParameters();
    }
}

Language 			StateMachine::getLanguage() const		{return language;}
Ontology 			StateMachine::getOntology() const		{return ontology;}
Encoding 			StateMachine::getEncoding() const		{return encoding;}
Protocol 			StateMachine::getProtocol() const		{return protocol;}
std::vector<StateMachine> 	StateMachine::getCancelMetaP() const		{return cancelMetaP;}
State* StateMachine::getCurrentState()
{
    std::vector<State>::iterator sit;
    sit = states.begin();
    for(;sit != states.end();++sit)
    {
        if(sit->getUID() == currentState)
            return &(*sit);
    }
    
    return NULL;
}

std::vector<AgentMapping> 	StateMachine::getInvolvedAgents () const	{return involvedAgents;}
AgentID 			StateMachine::getOwner() const		{return owner;}
std::vector<State> 		StateMachine::getStates() const		{return states;}
ConversationID		StateMachine::getConversationID() const		{return convid;}

void StateMachine::print()
{
    std::cout<<"========================"<<" StateMachine "<<"========================\n";
    std::cout<<"involvedAgents: ";
    for (std::vector<AgentMapping>::iterator it = involvedAgents.begin(); it != involvedAgents.end(); ++it)
        std::cout<< it->role <<" ";
    
    std::cout<<"\ncurrentState: ";
    std::cout<< currentState <<"\n";
    
    std::cout<<"owner: " << owner.getName() <<"\n";
    std::cout<<"active: "<<active<<"\n";
    std::cout<<"conversationOver: "<<conversationOver<<"\n";
    std::cout<<"ontology: "<<ontology<<"\n";
    std::cout<<"protocol: "<<protocol<<"\n";
    std::cout<<"language: "<<language<<"\n";
    std::cout<<"conversation ID: "<<convid<<"\n";
    std::cout<<"encoding: "<<encoding<<"\n\n";
    std::cout<<"********** States: **********\n";
    
    for (std::vector<State>::iterator it = states.begin(); it != states.end(); ++it)
        it->print();
    
    std::cout<<"========================"<<"=============="<<"========================\n";
}

bool StateMachine::isValid() const
{
    return isValidStateMachine;
}

void StateMachine::validate(bool associatedMachine)
{
    isValidStateMachine = true;

    std::vector<State>::iterator it = states.begin();
    std::vector<std::string> stateIds;

    for(; it != states.end(); ++it)
    {
        if(associatedMachine) 
            assert(it->getOwningMachine() == this);
        stateIds.push_back(it->getUID());
    }

    std::vector<std::string> requiredStates;
    for(it = states.begin(); it != states.end(); ++it)
    {
        std::vector<Transition> transitions = it->getTransitions();
        std::vector<Transition>::iterator tit = transitions.begin();
        for(; tit != transitions.end();++tit)
        {
           if(associatedMachine)
               assert(tit->getMachine() == this);

           std::string to = tit->getTo();
           std::string from = tit->getFrom(); 
        
           requiredStates.push_back(to);
           requiredStates.push_back(from);
        }
    }

    std::vector<std::string>::iterator requiredIt = requiredStates.begin();
    std::vector<std::string>::iterator availableIt;
    for(; requiredIt != requiredStates.end(); ++requiredIt)
    {
        availableIt == std::find(stateIds.begin(),  stateIds.end(), *requiredIt);
        if(availableIt == stateIds.end())
        {
            LOG_ERROR("Protocol definition is invalid. State with id %s is not defined.", requiredIt->c_str());
            isValidStateMachine = false;
            break;
        }
    }
}



} // end of acl
} // end of fipa
