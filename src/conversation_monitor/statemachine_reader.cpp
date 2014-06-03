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
const std::string StateMachineReader::file = std::string("file");
const std::string StateMachineReader::multiple = std::string("multiple");

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

    StateMachine statemachine = parseStateMachineNode(statemachineElement, protocolSpec);
    statemachine.generateDefaultTransitions();
    statemachine.generateDefaultStates();
    statemachine.updateRoles();
    statemachine.validate();
    statemachine.reset();
    return statemachine;
}

StateMachine StateMachineReader::parseStateMachineNode(TiXmlElement *statemachineElement, const std::string& protocolSpec)
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
        State state = parseStateNode(stateElement, protocolSpec);
        LOG_DEBUG_S << "Adding state: " << state.toString();
        statemachine.addState(state);
    }

    return statemachine;
}

State StateMachineReader::parseStateNode(TiXmlElement *stateElement, const std::string& protocolSpec)
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
    
    // Read subprotocols
    TiXmlElement *subProtocolElement = handleState.FirstChildElement(StateMachineReader::subprotocol).ToElement();
    for (; subProtocolElement != NULL; subProtocolElement = subProtocolElement->NextSiblingElement(StateMachineReader::subprotocol) )
    {
        EmbeddedStateMachine esm;
        StateMachine sm = parseSubProtocol(subProtocolElement, protocolSpec);
        esm.stateMachine = sm;
        
        const std::string* multipleAllowedP;
        multipleAllowedP = subProtocolElement->Attribute(StateMachineReader::multiple);
        if (multipleAllowedP)
        {
            const std::string multipleAllowed(multipleAllowedP->c_str());
            if (multipleAllowed == "yes" || multipleAllowed == "true" || multipleAllowed == "1")
            {
                esm.multiple = true;
            } else if (multipleAllowed == "no" || multipleAllowed == "false" || multipleAllowed == "0")
            {
                esm.multiple = false;
            } else {
                char buffer[512];
                sprintf(buffer, "Invalid value for attribute \"multiple\" of subprotocol node: %s", multipleAllowed.c_str());
                throw std::runtime_error(std::string(buffer));
            }
            
        } else {
            char buffer[512];
            sprintf(buffer, "Attribute could not be retrieved %s", StateMachineReader::multiple.c_str());
            throw std::runtime_error(std::string(buffer));
        }
        
        
        // Parse all mappings
        TiXmlHandle handleSubProtocol = TiXmlHandle(subProtocolElement);
        TiXmlElement *mappingElement = handleSubProtocol.FirstChildElement(StateMachineReader::mapping).ToElement();
        for (; mappingElement != NULL; mappingElement = mappingElement->NextSiblingElement(StateMachineReader::mapping) )
        {
            EmbeddedProtocolMapping mapping = parseEmbeddedProtocolMapping(mappingElement);
            esm.mappings.push_back(mapping);
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

EmbeddedProtocolMapping StateMachineReader::parseEmbeddedProtocolMapping(TiXmlElement* mappingElement)
{
    EmbeddedProtocolMapping epm;
    const std::string* from = mappingElement->Attribute(StateMachineReader::from);
    if (from != NULL)
    {
        epm.from = *from;
    } else {
        throw new std::runtime_error("StateMachineReader::parseEmbeddedProtocolMapping mapping is missing 'from' attribute");
    }
        
    const std::string* to = mappingElement->Attribute(StateMachineReader::to);
    if (to != NULL)
    {
        epm.to = *to;
    } else {
        throw new std::runtime_error("StateMachineReader::parseEmbeddedProtocolMapping mapping is missing 'to' attribute");
    }
    
    return epm;
}

StateMachine StateMachineReader::parseSubProtocol(TiXmlElement* subProtocolElement, const std::string& protocolSpec)
{
    // Get a valid file path
    boost::filesystem::path parentProtocolFile(protocolSpec);
    boost::filesystem::path dir = parentProtocolFile.parent_path();
    // Append the file
    const std::string* file = subProtocolElement->Attribute(StateMachineReader::file);
    dir /= *file;
    
    StateMachine subStateMachine;
    
    if (file != NULL)
    {
        subStateMachine = loadSpecification(dir.string());
    } else {
        throw new std::runtime_error("StateMachineReader::parseSubProtocol subprotocol is missing 'file' attribute");
    }
    
    return subStateMachine;
}


} //end of namespace acl
} //end of namespace fipa
