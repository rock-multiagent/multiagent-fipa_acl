#include "statemachine_builder.h"

#include <iostream>
#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <base/logging.h>
#include <boost/filesystem/operations.hpp>

namespace fs = boost::filesystem;

namespace fipa {
namespace acl {

const std::string StateMachineBuilder::from = 		std::string("from");
const std::string StateMachineBuilder::to = 		std::string("to");
const std::string StateMachineBuilder::target = 	std::string("target");
const std::string StateMachineBuilder::id = 		std::string("id");
const std::string StateMachineBuilder::final = 		std::string("final");
const std::string StateMachineBuilder::performative = 	std::string("performative");
const std::string StateMachineBuilder::initial = 	std::string("initial");

std::string StateMachineBuilder::resourceDir = std::string("");

std::map<std::string, StateMachine> StateMachineBuilder::availableStateMachines;

StateMachineBuilder::StateMachineBuilder() 
    : builtMachine()
    , preparedResourceDir(false)
{
}

void StateMachineBuilder::setProtocolResourceDir(const std::string& _resourceDir)
{
    fs::path protocolDir = fs::path(_resourceDir);
    if(fs::is_directory(protocolDir))
    {
        resourceDir = protocolDir.string();
    }
    LOG_INFO("Set protocol resource dir to: '%s' -- from '%s'", resourceDir.c_str(), _resourceDir.c_str());
}

void StateMachineBuilder::prepareProtocolsFromResourceDir()
{
    fs::path protocolDir = fs::path(resourceDir);
    LOG_INFO("Prepare protocols from: %s", protocolDir.string().c_str());
    if(fs::is_directory(protocolDir))
    {
        std::vector<fs::path> files;
        std::copy(fs::directory_iterator(protocolDir), fs::directory_iterator(), std::back_inserter(files));

        std::vector<fs::path>::iterator it = files.begin();
        for(;it != files.end(); ++it)
        {
            if(fs::is_regular_file(*it))
            {
                try {
                    // Extract protocol name
                    std::string protocolName = it->string();
                    size_t pos = protocolName.find_last_of("/");
                    protocolName.erase(0, pos+1);
                    StateMachine statemachine = getFunctionalStateMachine(protocolName);
                    LOG_INFO("Register protocol %s", protocolName.c_str());                    
                    availableStateMachines[protocolName] = statemachine;
                } catch(const std::runtime_error& e)
                {
                    LOG_ERROR("Error getting functional state machine: %s", e.what());
                }
            }
        }
    }

    preparedResourceDir = true;
}

StateMachine StateMachineBuilder::getStateMachine(const std::string& protocol)
{
    if(!preparedResourceDir)
        StateMachineBuilder::prepareProtocolsFromResourceDir();

    std::map<std::string, StateMachine>::iterator it = availableStateMachines.find(protocol);

    if(it != availableStateMachines.end())
    {
        return it->second;
    }

    LOG_WARN("StateMachine for protocol %s not found", protocol.c_str());
    throw std::runtime_error("State machine for requested protocol does not exist");
}

StateMachine StateMachineBuilder::getFunctionalStateMachine(const std::string& infile)
{
    locallyLoadSpecification(infile);
    builtMachine.generateDefaultStates();
    builtMachine.generateDefaultTransitions();
    builtMachine.validate();

    if(!builtMachine.isValid())
    {
        LOG_ERROR("Machine validation failed");
        throw std::runtime_error("State machine validation failed");
    }

    return builtMachine;
}

void StateMachineBuilder::locallyLoadSpecification(const std::string& infile)
{
    // Cleanup before instantiation of new statemachine
    roles.clear();
    states.clear();
    initialState = "";
    builtMachine = StateMachine();

    ///call the other loadSpecification method and just discard the returned value(i.e: run it only to parameterize builtMachine)
    loadSpecification(infile);
}

StateMachine StateMachineBuilder::loadSpecification(const std::string& infile)
{
    boost::filesystem::path protocolDir(resourceDir);
    boost::filesystem::path infilePath = operator/(protocolDir, infile);
    
    std::string protocolSpec = infilePath.string();
    TiXmlDocument file = TiXmlDocument(protocolSpec.c_str());
    
    if (!file.LoadFile())
    {
        LOG_ERROR("error loading the spec file: %s. Please use setProtocolResourceDir(const std::string&) to specify the location of your protocol files", protocolSpec.c_str());
        throw std::runtime_error("Error loading the specification file");
    }

    LOG_DEBUG("loadSpecification: file opened: %s", infile.c_str());
    TiXmlElement *sm = file.RootElement();
    parseStateMachineNode(sm);
    
    return builtMachine;
}

void StateMachineBuilder::parseStateMachineNode(TiXmlElement *sm)
{
    const char *value;
    
    value = sm->Attribute(StateMachineBuilder::initial.c_str());
    LOG_DEBUG("parseStateMachineNode: retrieved value of the \"initial\" state: %s", value);
    if (value)
        initialState = std::string(value);
    else
        throw std::runtime_error("Attribute of initial could not be retrieved");
        
    TiXmlHandle handleSm = TiXmlHandle(sm);
    TiXmlElement* child = handleSm.FirstChild( "state" ).ToElement();
    
    if (child)
    {
        LOG_DEBUG("parseStateMachineNode: found first state element");
    } else {
        LOG_WARN("pareStateMachineNode: parsestate element is NULL");
    }
    
    addStates(child);
    builtMachine.setInitialState(initialState);
    addInvolvedAgentsMap();
}

State StateMachineBuilder::parseStateNode(TiXmlElement *st)
{
    State state;
    const char *value;
    value = st->Attribute(StateMachineBuilder::id.c_str());
    if (value) 
    {
        state.setUID(std::string(value));
    } else 
    {
        char buffer[512];
        sprintf(buffer, "Attribute could not be retrieved %s", StateMachineBuilder::id.c_str());
        throw std::runtime_error(std::string(buffer));
    }
    
    value = NULL;
    value = st->Attribute(StateMachineBuilder::final.c_str());
    if (value)
    {
        if (!strcmp(value,"yes"))
        {
	  state.setFinal(true);
        } else if (!strcmp(value,"no"))
        {
	    state.setFinal(false);
	} else {
            char buffer[512];
            sprintf(buffer, "Invalid value for attribute \"final\" of state node: %s", value);
	    throw std::runtime_error(std::string(buffer));
        }
    }

    TiXmlHandle handleState = TiXmlHandle(st);
    TiXmlElement *trans = handleState.FirstChildElement("transition").ToElement();
    for (; trans != NULL; trans = trans->NextSiblingElement("transition") )
    {
        Transition t = Transition(parseTransitionNode(trans));
        state.addTransition(t);

        LOG_DEBUG("parseStateNode: state: %s -> transition added: from %s, to %s, next state: %s, performative %s",
                 state.getUID().c_str(),
                 t.getFrom().c_str(),
                 t.getTo().c_str(),
                 t.getNextStateName().c_str(),
                 t.getExpectedPerformative().c_str()
                );
    }
    
    //TODO: implement specification for subprotocols
    return state;
}
void StateMachineBuilder::addStates(TiXmlElement *st)
{
    const char *value = NULL;
    value = st->Attribute(StateMachineBuilder::id.c_str());
    if (value)
    {
        std::vector<std::string>::iterator it = find(states.begin(), states.end(), std::string(value));
        if ( it == states.end() )
        {
	  states.push_back(std::string(value));
        } else
        {
          char buffer[512];
          sprintf(buffer, "Multiple state with \"id\" %s. State id should be unique", value);
	  throw std::runtime_error(std::string(buffer));
        }
    } else {
        throw std::runtime_error("State node has no \"id\" attribute");
    }
   
    // Add current state node 
    State state = parseStateNode(st);
    builtMachine.addState(state);        

    // Add all state nodes at the same level 
    TiXmlElement *next = st;
    while( (next = next->NextSiblingElement("state")) != NULL )
    {
        State state = parseStateNode(next);
        builtMachine.addState(state);        
    }
}

Transition StateMachineBuilder::parseTransitionNode(TiXmlElement *trans)
{
    Transition ret;
    const char *value = NULL;
    value = trans->Attribute(StateMachineBuilder::from.c_str());
    if ( value )
    {
        ret.setFrom(std::string(value));

        std::vector<std::string>::iterator it = find (roles.begin(), roles.end(), std::string(value));
        if ( it == roles.end())
	  roles.push_back(std::string(value));
    }
    else {
        throw new std::runtime_error("StateMachineBuilder::parseTransitionsNode Transitions is missing 'from' attribute");
    }
        
    value = NULL;
    value = trans->Attribute(StateMachineBuilder::to.c_str());
    if (value)
    {
        ret.setTo(std::string(value));
        std::vector<std::string>::iterator it = find (roles.begin(), roles.end(), std::string(value));
        if ( it == roles.end())
	  roles.push_back(std::string(value));
    }
    else
    {
        throw new std::runtime_error("StateMachineBuilder::parseTransitionsNode transition is missing 'to' attribute");
    }
    
    value = NULL;
    value = trans->Attribute(StateMachineBuilder::target.c_str());
    if (value)
    {
	ret.setNextStateName(std::string(value));
    } else {
        LOG_ERROR("Could not retrieve attribute for target state: %s", StateMachineBuilder::target.c_str());
        throw std::runtime_error("Could not retrieve attribute for target state");
    }

    value = NULL;
    value = trans->Attribute(StateMachineBuilder::performative.c_str());
    if (value)
    {
        ret.setExpectedPerformative(std::string(value));
    }
    else
    {
        throw new std::runtime_error("Transition performative does not exist");
    }
    
    return ret;
}

void StateMachineBuilder::addInvolvedAgentsMap()
{
    std::vector<std::string>::iterator it;
    for (it = roles.begin(); it != roles.end(); ++it)
    {
        std::string newrole = Role(*it);
        builtMachine.addRole(newrole);
    }
}

} //end of namespace acl
} //end of namespace fipa
