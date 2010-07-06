
#include "StateMachine"

namespace fipa {
namespace acl {

const std::string NOT_UNDERSTOOD = std::string("not-understood");
const std::string CONV_CANCELLED = std::string("conv-cancelled");
const std::string INITIAL = std::string("initial");
void StateMachine::initializeObjectFields()
{
    
    
    involvedAgents.clear();
    states.clear();
    owner = Agent();
    conversationOver = false;

}

void StateMachine::generateDefaultStates()
{
    //currentState = NULL;
    State initial = State(StateMachine::INITIAL);
    initial.setFinal(false);
    states.push_back(initial);
    currentState = &(*find(states.begin(),states.end(),StateMachine::INITIAL));
    
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
        if (!it->getUid().compare(StateMachine::NOT_UNDERSTOOD)) continue;
        //if (!it->getUid().compare(StateMachine::CONV_CANCELLED)) continue;
        //if (!it->getUid().compare(StateMachine::INITIAL) continue;
        
        it->generateDefaultTransitions();        
    }
}
//&&&&&&&&&&&&&NOTE:remember to treat not understood transitions differently when executing as well as when loading--not so sure i still need to(on exec)&&&&&&&&&&//
//~~~~&&&&&&&&&&&&&NOTE:remember to check for duplicates when adding a transition to a state&&&&&&&&&&//

int StateMachine::createAndStartNewCancelMetaProtocol(ACLMessage &msg)
{
    cancelMetaP.push_back(generateCancelMetaProtocol(std::string("with")));
    return cancelMetaP.back().startMachine(msg);
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
        temp.generateDefaultStates();
        
        State first = State(std::String("1"));
        first.setFinal(false);
        State second = State(std::String("2"));
        second.setFinal(true);
        
        temp.addState(first);
        temp.addState(second);
        
        Transition t1 = Transition();
        t1.setExpectedPerformative(ACLMessage::perfs[ACLMessage::CANCEL]);
        t1.setFrom(StateMachine::INITIATOR);
        t1.setTo(with);
        t1.setNextStateName(std::string("1"));
        //t1.setMessageParity(true);
        temp.getStateByName(StateMachine::INITIAL)->addTranstition(t1);
        
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
    initializeFields();
    owner = _owner;
    
}

int StateMachine::startMachine(ACLmessage &msg)
{
    generateRoles(msg);
    
}

int StateMachine::consumeMessage(ACLMessage msg)
{
    std::vector<StateMachine>::iterator it;
    for (it = cancelMetaP.begin(); it != cancelMetaP.end(); it++)
    {
        if (it->isActive() )
	  if (it->consumeMessage() == 0) return 0;
    }
    if (!msg.getPerformative().compare(ACLMessage::CANCEL) )
        if (!getAgentRole(msg.getSender()).compare(StateMachine::INITIATOR) )
	  return createAndStartNewMetaCancelP(msg);
        //else return 1;
    //else;
    return currentState->consumeMessage(msg);
}

State* StateMachine::getStateByName(std::string _name)
{
    std::vector<State>::iterator it = find(states.begin(),states.end(),_name);
    if (it == states.end()) return NULL;
    return (&(*it));
}
void StateMachine::removeInterlocutor(AgentAID &agent)
{
    std::vector<AgentMapping>::iterator it;
    for (it = involvedAgents.begin(); it != involvedAgents.end(); it++)
    {
        if (it->agent == agent)
	  if (it->check) involvedAgents.erase(it);
    }
}
void StateMachine::removeInterlocutor(std::vector<AgentAID> &agents)
{
    std::vector<AgentAID>::iterator it;
    for (it = agents.begin(); it != agents.end(); it++)
        removeInterlocutor(*it);
}

bool StateMachine::setRole(Role myrole,AgentAID myagent)
{
    std::vector<AgentMapping>::iterator it;
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
    for (agit = agents.begin(); agit != agents.end(); it++)
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

void removeInterlocutor(AgentAID &ag)
{
    std::vector<AgentMapping>::iterator it;
    bool flag = true;
    while (flag)
    {
        flag = true;
        for (it = involvedAgents.begin(); it != involvedAgents.end(); it++)
        {
	  if (it->agent == ag) 
	  {
	      involvedAgents.erase(it);
	      flag = false;
	      it = involvedAgents.end();
	  }
        }
    }
}

void removeInterlocutor(std::vector<AgentAID> &agents)
{
    //for_each(agents.begin(), agents.end(), removeInterlocutor);
    std::vector<AgentAID>::iterator it;
    for (it = agents.begin(); it != agents.end(); it++)
        removeInterlocutor(*it);
}


} // end of acl
} // end of fipa