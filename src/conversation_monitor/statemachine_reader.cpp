#include "statemachine_reader.h"

#include <iostream>
#include <algorithm>
#include <cstring>
#include <base/logging.h>

#include <tinyxml.h>
#include <boost/filesystem/path.hpp>

#include "transition.h"


namespace fipa {
namespace acl {

const std::string StateMachineReader::transition = std::string("transition");    
const std::string StateMachineReader::from = std::string("from");
const std::string StateMachineReader::to = std::string("to");
const std::string StateMachineReader::target =	std::string("target");
const std::string StateMachineReader::id = std::string("id");
const std::string StateMachineReader::final = std::string("final");
const std::string StateMachineReader::performative = std::string("performative");
const std::string StateMachineReader::initial = std::string("initial");
const std::string StateMachineReader::subprotocol = std::string("subprotocol");
const std::string StateMachineReader::mapping = std::string("mapping");
const std::string StateMachineReader::name = std::string("name");
const std::string StateMachineReader::proxiedTo = std::string("proxied_to");

StateMachine StateMachineReader::loadSpecification(const std::string& protocolSpec)
{
    {
        FILE* file = fopen(protocolSpec.c_str(), "r");
        if(file)
        {
            fclose(file);
        } else {
            LOG_ERROR_S << "error loading the spec file: '" << protocolSpec << "'. File does not exist.";
            throw std::runtime_error("Error loading the specification file: '" + protocolSpec + "' -- file does not exist");
        }
    }

    TiXmlDocument file = TiXmlDocument(protocolSpec.c_str());
    
    if (!file.LoadFile())
    {
        LOG_ERROR_S << "error loading the spec file: '" << protocolSpec << "'. Please use setProtocolResourceDir(const std::string&) instead to specify the location of your protocol files";
        throw std::runtime_error("Error loading the specification file: '" + protocolSpec + "' -- tinyxml failed to load the file");
    }

    LOG_DEBUG_S << "loadSpecification: specification file 'protocolSpec'";
    TiXmlElement* statemachineElement = file.RootElement();

    StateMachine statemachine = parseStateMachineNode(statemachineElement);
    // set protocol
    boost::filesystem::path path(protocolSpec);
    statemachine.setProtocol( path.filename().string());
    
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
    if (initialState)
    {
        statemachine.setInitialState(std::string(initialState));
    } else {
        throw std::runtime_error("Attribute of initial could not be retrieved");
    }
    LOG_DEBUG_S << "parseStateMachineNode: retrieved value of the \"initial\" state: " << std::string(initialState);
        
    TiXmlHandle handleSm = TiXmlHandle(statemachineElement);
    TiXmlElement* stateElement = handleSm.FirstChild( "state" ).ToElement();
    if (stateElement)
    {
        LOG_DEBUG_S << "parseStateMachineNode: found first state element";
    } else {
        LOG_WARN_S << "parseStateMachineNode: no state element found";
    }
   
    // read states nodes 
    for (; stateElement != NULL; stateElement = stateElement->NextSiblingElement("state") )
    {
        State state = parseStateNode(stateElement);
        LOG_DEBUG_S << "Adding state: " << state.toString();
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
    TiXmlElement *transitionElement = handleState.FirstChildElement(StateMachineReader::transition).ToElement();
    for (; transitionElement != NULL; transitionElement = transitionElement->NextSiblingElement(StateMachineReader::transition) )
    {
        Transition t = parseTransitionNode(transitionElement);
        state.addTransition(t);

        LOG_DEBUG_S << "parseStateNode: state: " << state.getId() << " -> transition added: from (Role) " << t.getSenderRole().getId()
                    << ", to (Role) " << t.getReceiverRole().getId() << ", target state: " << t.getTargetStateId() << ", performative "
                    << t.getPerformativeRegExp();
    }
    
    // TODO allow only 1 subProtocol per state? Throw otherwise.
    // Also adapt the structs, etc.
    
    // Read subprotocols
    TiXmlElement *subProtocolElement = handleState.FirstChildElement(StateMachineReader::subprotocol).ToElement();
    for (; subProtocolElement != NULL; subProtocolElement = subProtocolElement->NextSiblingElement(StateMachineReader::subprotocol) )
    {
        EmbeddedStateMachine esm;
        
        const std::string* name = subProtocolElement->Attribute(StateMachineReader::name);
        if (name != NULL)
        {
            esm.name = *name;
        } else {
            throw new std::runtime_error("StateMachineReader::parseStateNode subprotocol is missing 'name' attribute");
        }
        
        const std::string* from = subProtocolElement->Attribute(StateMachineReader::from);
        if (from != NULL)
        {
            esm.from = *from;
            esm.fromRole = Role( std::string(from->c_str()));
        } else {
            throw new std::runtime_error("StateMachineReader::parseStateNode mapping is missing 'from' attribute");
        }
        
        const std::string* proxiedTo = subProtocolElement->Attribute(StateMachineReader::proxiedTo);
        if (proxiedTo != NULL)
        {
            esm.proxiedTo = *proxiedTo;
            esm.proxiedToRole = Role( std::string(proxiedTo->c_str()));
        }
        
        state.addEmbeddedStateMachine(esm);
        LOG_DEBUG_S << "parseStateNode: state: " << state.getId() << " -> subprotocol added:\n" << esm.toString();
    }
    
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
        LOG_ERROR_S << "Could not retrieve attribute for target state: " << StateMachineReader::target;
        throw std::runtime_error("Could not retrieve attribute for target state");
    }

    const char* performative = NULL;
    performative = transitionElement->Attribute(StateMachineReader::performative.c_str());
    if (performative)
    {
        transition.setPerformativeRegExp( performative );
    } else {
        throw new std::runtime_error("StateMachineReader::parseTransitionsNode transition is missing 'performative' attribute");
    }
    
    return transition;
}

} //end of namespace acl
} //end of namespace fipa
