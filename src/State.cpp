
#include "State.h"

namespace fipa {
namespace acl {
    

State::State()
{
    final = false;
    name.clear();
    transitions.clear();
    subSM.clear();    
}

void State::addTransition(Transition t)
{
    std::vector<Transition>::iterator it;
    for (it = transitions.begin(); it != transitions.end(); it++)
        if (unloadedEqual(*it,t) ) return;
    transitions.push_back(t);
}

void State::generateDefaultTransitions()
{
        std::vector<Transition>::iterator trit;
        for (trit = transitions.begin(); trit != transitions.end(); trit++)
        {
	  // i don't generate a not-understood transition for the not-understood message...
	  if (!trit->getExpectedPerformative().compare(ACLMessage::perfs[ACLMessage::NOT_UNDERSTOOD]) ) continue;
	  
	  //if (!trit->getExpectedPerformative().compare(ACLMessage::perfs[ACLMessage::CANCEL]) ) continue;
	  /*
	  std::map<AgentAID,bool>* mymapS = trit->getExpectedSendersByRef();
	  std::map<AgentAID,bool>*mymapR = trit->getExpectedRecepientsByRef();
	  
	  for (std::map<AgentAID,bool>::iterator itS = mymapS->begin(); itS != mymapS.end(); itS++)
	  {
	      for (std::map<AgentAID,bool>::iterator itR = mymapR->begin(); itR != mymapR.end(); itR++)
	      {
		Transition t = Transition();
		t.setExpectedPerformative(ACLMessage::perfs[ACLMessage::NOT_UNDERSTOOD]);
		t.set
	      }
	  }
	  */
	  Transition t = Transition();
	  t.setExpectedPerformative(ACLMessage::perfs[ACLMessage::NOT_UNDERSTOOD]);
	  t.setFrom(trit->getTo());
	  t.setTo(trit->getFrom());
	  t.setNextStateName(StateMachine::NOT_UNDERSTOOD);
	  //t.setMessageParity(true);
	  trit->nextState->addTransition(t);
        }
}

int State::consumeMessage(ACLMessage &msg)
{
    if (!subSM.empty())
    {
        bool found_one = false;
        bool stillActiveSubSM = false;
        std::vector<StateMachine>::iterator smit;
        for (smit = subSM.begin(); smit != subSM.end(); smit++)
        {
	  if (smit->isActive() )
	      if (smit->IsConversationOver() ) continue;
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
    std::vector<Transition>::iterator it;
    for (it = transitions.begin(); it != transitions.end(); it++)
    {
        if (it->consumeMessage(msg) == 0) return 0;
    }
    return 1;
}

} // end of acl
} // end of fipa