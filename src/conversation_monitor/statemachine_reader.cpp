#include "statemachine_reader.h"

#include <iostream>
#include <algorithm>
#include <cstring>
#include <base/logging.h>

#include <tinyxml.h>

#include "transition.h"


namespace fipa {
namespace acl {

const std::string StateMachineReader::from = std::string("from");
const std::string StateMachineReader::to = std::string("to");
const std::string StateMachineReader::target =	std::string("target");
const std::string StateMachineReader::id = std::string("id");
const std::string StateMachineReader::final = std::string("final");
const std::string StateMachineReader::performative = std::string("performative");
const std::string StateMachineReader::initial = std::string("initial");

StateMachine StateMachineReader::loadSpecification(const std::string& protocolSpec)
{
    {
        FILE* file = fopen(protocolSpec.c_str(), "r");
        if(file)
        {
            fclose(file);
        } else {
            LOG_ERROR("error loading the spec file: '%s'. File does not exist.", protocolSpec.c_str());
            throw std::runtime_error("Error loading the specification file: '" + protocolSpec + "' -- file does not exist");
        }
    }

    TiXmlDocument file = TiXmlDocument(protocolSpec.c_str());
    
    if (!file.LoadFile())
    {
        LOG_ERROR("error loading the spec file: '%s'. Please use setProtocolResourceDir(const std::string&) instead to specify the location of your protocol files", protocolSpec.c_str());
        throw std::runtime_error("Error loading the specification file: '" + protocolSpec + "' -- tinyxml failed to load the file");
    }

    LOG_DEBUG("loadSpecification: specification file '%s'", protocolSpec.c_str());
    TiXmlElement* statemachineElement = file.RootElement();

    StateMachine statemachine = parseStateMachineNode(statemachineElement);
    statemachine.generateDefaultTransitions();
    statemachine.generateDefaultStates();
    statemachine.updateRoles();
    statemachine.validate();
    statemachine.reset();
    return statemachine;
}

StateMachine StateMachineReader::parseStateMachineNode(TiXmlElement *statemachineElement)
{
    StateMachine statemachine;

    const char *initialState;
    
    initialState = statemachineElement->Attribute(StateMachineReader::initial.c_str());
    LOG_DEBUG("parseStateMachineNode: retrieved value of the \"initial\" state: %s", initialState);
    if (initialState)
    {
        statemachine.setInitialState(std::string(initialState));
    } else {
        throw std::runtime_error("Attribute of initial could not be retrieved");
    }
        
    TiXmlHandle handleSm = TiXmlHandle(statemachineElement);
    TiXmlElement* stateElement = handleSm.FirstChild( "state" ).ToElement();
    
    if (stateElement)
    {
        LOG_DEBUG("parseStateMachineNode: found first state element");
    } else {
        LOG_WARN("parseStateMachineNode: no state element found");
    }
   
    // read states nodes 
    for (; stateElement != NULL; stateElement = stateElement->NextSiblingElement("state") )
    {
        State state = parseStateNode(stateElement);
        LOG_DEBUG("Adding state '%s", state.toString().c_str());
        statemachine.addState(state);
    }

    return statemachine;
}

State StateMachineReader::parseStateNode(TiXmlElement *stateElement)
{
    State state;

    const char *stateId;
    stateId = stateElement->Attribute(StateMachineReader::id.c_str());
    if (stateId)
    {
        state.setId(std::string(stateId));
    } else {
        char buffer[512];
        sprintf(buffer, "Attribute could not be retrieved %s", StateMachineReader::id.c_str());
        throw std::runtime_error(std::string(buffer));
    }
    
    const char* final = NULL;
    final = stateElement->Attribute(StateMachineReader::final.c_str());
    if (final)
    {
        std::string isFinal(final);
        if (isFinal == "yes" || isFinal == "true" || isFinal == "1")
        {
	  state.setFinal(true);
        } else if (isFinal == "no" || isFinal == "false" || isFinal == "0")
        {
	    state.setFinal(false);
	} else {
            char buffer[512];
            sprintf(buffer, "Invalid value for attribute \"final\" of state node: %s", final);
	    throw std::runtime_error(std::string(buffer));
        }
    }

    // Read transitions
    TiXmlHandle handleState = TiXmlHandle(stateElement);
    TiXmlElement *transitionElement = handleState.FirstChildElement("transition").ToElement();
    for (; transitionElement != NULL; transitionElement = transitionElement->NextSiblingElement("transition") )
    {
        Transition t = parseTransitionNode(transitionElement);
        state.addTransition(t);

        LOG_DEBUG("parseStateNode: state: %s -> transition added: from (Role) %s, to (Role) %s, target state: %s, performative %s",
                 state.getId().c_str(),
                 t.getSenderRole().getId().c_str(),
                 t.getReceiverRole().getId().c_str(),
                 t.getTargetStateId().c_str(),
                 PerformativeTxt[t.getPerformative()].c_str()
                );
    }
    
    //TODO: implement specification for subprotocols
    return state;
}

Transition StateMachineReader::parseTransitionNode(TiXmlElement *transitionElement)
{
    Transition transition;
    const char *from = NULL;
    from = transitionElement->Attribute(StateMachineReader::from.c_str());
    if ( from )
    {
        transition.setSenderRole( Role( std::string(from) ));
    } else {
        throw new std::runtime_error("StateMachineReader::parseTransitionsNode Transitions is missing 'from' attribute");
    }
        
    const char* to = NULL;
    to = transitionElement->Attribute(StateMachineReader::to.c_str());
    if (to)
    {
        transition.setReceiverRole( Role( std::string(to) ) );
    } else {
        throw new std::runtime_error("StateMachineReader::parseTransitionsNode transition is missing 'to' attribute");
    }
    
    const char* targetState = NULL;
    targetState = transitionElement->Attribute(StateMachineReader::target.c_str());
    if (targetState)
    {
	transition.setTargetState(std::string(targetState));
    } else {
        LOG_ERROR("Could not retrieve attribute for target state: %s", StateMachineReader::target.c_str());
        throw std::runtime_error("Could not retrieve attribute for target state");
    }

    const char* performative = NULL;
    performative = transitionElement->Attribute(StateMachineReader::performative.c_str());
    if (performative)
    {
        transition.setPerformative( ACLMessage::performativeFromString(performative) );
    } else {
        throw new std::runtime_error("Transition performative does not exist");
    }
    
    return transition;
}

} //end of namespace acl
} //end of namespace fipa
