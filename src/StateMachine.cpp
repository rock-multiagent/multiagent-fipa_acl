
#include "StateMachine.h"
#include "State.h"

namespace fipa {
namespace acl {

const std::string StateMachine::NOT_UNDERSTOOD = std::string("not-understood");
const std::string StateMachine::CONV_CANCELLED = std::string("conv-cancelled");
const std::string StateMachine::INITIAL = std::string("initial");
const std::string StateMachine::INITIATOR = std::string("initiator");
void StateMachine::initializeObjectFields()
{
    
    
    involvedAgents.clear();
    states.clear();
    currentState = NULL;
    owner = AgentAID();
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

bool StateMachine::setInitialState(std::string _uid)
{
    if (active) return false;
    State *s = getStateByName(_uid);
    if (!s) return false;
    currentState = s;
    return true;
}

bool StateMachine::setOwner(AgentAID _owner)
{
    if (active) return false;
    owner = _owner;
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

int StateMachine::createAndStartNewCancelMetaProtocol(ACLMessage &msg)
{
    std::vector<AgentAID> interlocutors = msg.getAllReceivers();
    for (std::vector<AgentAID>::iterator it = interlocutors.begin(); it != interlocutors.end(); it++)
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
}

StateMachine::StateMachine(AgentAID _owner)
{
    initializeObjectFields();
    owner = _owner;
    
}

int StateMachine::startMachine(ACLMessage msg)
{
    int x;
    active = true;
    if ((x = consumeMessage(msg)) != 0) return x;
    convid = msg.getConversationID();
    protocol = msg.getProtocol();
    encoding = msg.getEncoding();
    language = msg.getLanguage();
    ontology = msg.getOntology();
    
    return 0;
    
}

int StateMachine::consumeMessage(ACLMessage msg)
{
    int x = 0; // return code received from the current state
    std::vector<StateMachine>::iterator it;
    for (it = cancelMetaP.begin(); it != cancelMetaP.end(); it++)
    {
        if (it->isActive() )
        {	  
	  if (it->consumeMessage(msg) == 0) return 0;
	  //x= 1;
        }
    }
    if (!msg.getPerformative().compare(ACLMessage::perfs[ACLMessage::CANCEL]) )
        if (!getAgentRole(msg.getSender()).compare(StateMachine::INITIATOR) )
	  return createAndStartNewCancelMetaProtocol(msg);
        else return 1;
    else;
    
    if (currentState->consumeMessage(msg) == 0)   
        if (currentState->getFinal() ) { conversationOver = true; active = false; return 0;}
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

State* StateMachine::getStateByName(std::string _name)
{
    std::vector<State>::iterator it = find(states.begin(),states.end(),_name);
    if (it == states.end()) return NULL;
    return (&(*it));
}
void StateMachine::removeInterlocutor(AgentAID ag)
{
    std::vector<AgentMapping>::iterator it;
    for (it = involvedAgents.begin(); it != involvedAgents.end(); it++)
    {
        if (it->agent == ag)
	  if (it->check) { involvedAgents.erase(it); break; }
    }
}
void StateMachine::removeInterlocutor(std::vector<AgentAID> agents)
{
    std::vector<AgentAID>::iterator it;
    for (it = agents.begin(); it != agents.end(); it++)
        removeInterlocutor(*it);
}

bool StateMachine::setRole(Role myrole,AgentAID myagent)
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
	      return true;
	  }
    }
    return false;
}
bool StateMachine::setRole(Role myrole, std::vector<AgentAID> agents)
{
    //if (checkIfRoleSet(myrole) ) return false;
    std::vector<AgentAID>::iterator agit;
    for (agit = agents.begin(); agit != agents.end(); agit++)
    {
        if (checkIfAgentAssigned(*agit) ) return false;
    }
    
    std::vector<AgentMapping>::iterator it;
    for (it = involvedAgents.begin(); it != involvedAgents.end(); it++)
        if (!it->role.compare(myrole) )
	  if (!it->check) 
	      involvedAgents.erase(it);
	  
    for (agit = agents.begin(); agit != agents.end(); it++)
    {
        AgentMapping element;
        element.role = myrole;
        element.agent = *agit;
        element.check = true;
        involvedAgents.push_back(element);
    }
}

bool StateMachine::checkIfAgentAssigned(AgentAID ag)
{
    std::vector<AgentMapping>::iterator it;
    for (it = involvedAgents.begin(); it != involvedAgents.end(); it++)
    {
        if (it->agent == ag)
	  if (it->check == true) return true;
    }
    return false;
}

bool StateMachine::checkIfRoleSet(Role &myrole)
{
    std::vector<AgentMapping>::iterator it;
    for (it = involvedAgents.begin(); it != involvedAgents.end(); it++)
    {
        if (!it->role.compare(myrole) )
	  return it->check;
    }
    return false;
}

bool StateMachine::isConversationOver()
{
    return conversationOver;
}
bool StateMachine::isActive()
{
    return active;
}
bool StateMachine::checkIfRoleExists(Role myrole)
{
    std::vector<AgentMapping>::iterator it;
    for (it = involvedAgents.begin(); it != involvedAgents.end(); it++)
    {
        if (!it->role.compare(myrole) ) return true;
    }
    return false;
}

bool StateMachine::addState(State _state)
{
    std::string name = _state.getUID();
    if (find(states.begin(),states.end(),name) != states.end() ) {states.push_back(_state); return true;}
    return false;
}

Role StateMachine::getAgentRole(AgentAID ag)
{
    std::vector<AgentMapping>::iterator it;
    for (it = involvedAgents.begin(); it != involvedAgents.end(); it++)
        if (it->agent == ag) return it->role;
        return std::string("");
}

void StateMachine::addRole(const Role& role)
{

}




} // end of acl
} // end of fipa
