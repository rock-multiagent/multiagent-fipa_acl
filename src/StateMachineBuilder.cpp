#include "StateMachineBuilder.h"

#include <iostream>
#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <base/logging.h>

namespace fipa {
namespace acl {

const std::string StateMachineBuilder::from = 		std::string("from");
const std::string StateMachineBuilder::to = 		std::string("to");
const std::string StateMachineBuilder::target = 	std::string("target");
const std::string StateMachineBuilder::id = 		std::string("id");
const std::string StateMachineBuilder::final = 		std::string("final");
const std::string StateMachineBuilder::performative = 	std::string("performative");
const std::string StateMachineBuilder::initial = 	std::string("initial");

StateMachineBuilder::StateMachineBuilder()
{
    roles.clear();
    states.clear();
    initialState.clear();
}

StateMachine StateMachineBuilder::getFunctionalStateMachine(std::string infile)
{
    localyLoadSpecification(infile);
    
    builtMachine.generateDefaultStates();
    builtMachine.generateDefaultTransitions();
        
    return builtMachine;
}

void StateMachineBuilder::localyLoadSpecification(std::string infile)
{
    ///call the other loadSpecification method and just discard the returned value(i.e: run it only to parameterize builtMachine)
    StateMachine dummy = loadSpecification(infile);
}

StateMachine StateMachineBuilder::loadSpecification(std::string infile)
{
    TiXmlDocument file = TiXmlDocument(infile.c_str());
    
    if (!file.LoadFile())
    {
        LOG_ERROR("error loading the spec file");
        exit(1);
    }
    builtMachine = StateMachine();
    LOG_DEBUG("loadSpecification: file opened: %s", infile.c_str());
    TiXmlElement *sm = file.RootElement();
    parseStateMachineNode(sm);
    
    return builtMachine;
}

void StateMachineBuilder::parseStateMachineNode(TiXmlElement *sm)
{
    const char *value;
    
    value = sm->Attribute(StateMachineBuilder::initial.c_str());
    LOG_DEBUG("parseStateMachineNode: retrieved value of the \"initial\" parameter");
    if (value)
        initialState = std::string(value);
    else
        throw std::runtime_error("Attribute of initial could not be retrieved");
        
    LOG_DEBUG("parseStateMachineNode: saved the initial state of the machine");
    
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
    State ret;
    LOG_DEBUG("parseStateNode: called");
    
    const char *value;
    value = st->Attribute(StateMachineBuilder::id.c_str());
    //if (value) ---- should not be necessary to check because it is already checked in the caller function(parseStates())
        ret.setUID(std::string(value));
        LOG_DEBUG("\t\t#uid set: %s", value);
    //else;
        //TODO: throw some exception here
    
    value = NULL;
    value = st->Attribute(StateMachineBuilder::final.c_str());
    LOG_DEBUG("\t\t#value field retrieved for final: %s", value);
    if (value)
    {
        if (!strcmp(value,"yes"))
	  ret.setFinal(true);
        else
	  if (!strcmp(value,"no"))
	      ret.setFinal(false);
	  else;
	      throw std::runtime_error("Invalid value for attribute \"final\" of state node");
    }
    LOG_DEBUG("\t\t#set the final variable");
        
    ret.setOwningMachine(&builtMachine);
    
    TiXmlHandle handleState = TiXmlHandle(st);
    TiXmlElement *trans = handleState.FirstChildElement("transition").ToElement();
    for (trans; trans != NULL; trans = trans->NextSiblingElement("transition") )
    {
        Transition t = Transition(parseTransitionNode(trans));
        t.setOwningState(&ret);
        ret.addTransition(t);

        LOG_DEBUG("\t\tparseStateNode: transition added: from %s, to %s, next state: %s, performative %s",
                 t.getFrom().c_str(),
                 t.getTo().c_str(),
                 t.getNextStateName().c_str(),
                 t.getExpectedPerformative().c_str()
                );
    }
    
    //TODO: implement specification for subprotocols
    return ret;
}
void StateMachineBuilder::addStates(TiXmlElement *st)
{
    LOG_DEBUG("addStates called");
    TiXmlElement *next;
    const char *value = NULL;
    value = st->Attribute(StateMachineBuilder::id.c_str());
    if (value)
    {
        LOG_DEBUG("\t#retrieved id of the state");
        std::vector<std::string>::iterator it = find(states.begin(), states.end(), std::string(value));
        if ( it == states.end() )
	  states.push_back(std::string(value));
        else;
	  throw std::runtime_error("\"id\" field of states should be unique");
    }
    else;
        throw std::runtime_error("State node has no \"id\" attribute");
    
    if ( (next = st->NextSiblingElement("state")) != NULL )
    {
        addStates(next);
        State state = parseStateNode(st);
        builtMachine.addState(state);        
    }
    else
    {
        State state = parseStateNode(st);
        builtMachine.addState(state);
    }
}

Transition StateMachineBuilder::parseTransitionNode(TiXmlElement *trans)
{
    Transition ret;
    const char *value = NULL;
    LOG_DEBUG("parseTransitionNode called");
    value = trans->Attribute(StateMachineBuilder::from.c_str());
    if ( value )
    {
        std::vector<std::string>::iterator it = find (roles.begin(), roles.end(), std::string(value));
        ret.setFrom(std::string(value));
        if ( it == roles.end())
	  roles.push_back(std::string(value));
    }
    else;
        //TODO: throw some exception here
        
    value = NULL;
    value = trans->Attribute(StateMachineBuilder::to.c_str());
    if (value)
    {
        ret.setTo(std::string(value));
        std::vector<std::string>::iterator it = find (roles.begin(), roles.end(), std::string(value));
        if ( it == roles.end())
	  roles.push_back(std::string(value));
    }
    else;
        //TODO: throw some exception here
    
    value = NULL;
    value = trans->Attribute(StateMachineBuilder::target.c_str());
    if (value)
    {
        std::vector<std::string>::iterator it = find(states.begin(), states.end(), std::string(value));
        if ( it != states.end() )
	  ret.setNextStateName(std::string(value));
        else
        {
          LOG_ERROR("Transition to an unknown state in the specification file: %s", value);
          throw std::runtime_error("No such state in the specification file");
        }
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
    for (it = roles.begin(); it != roles.end(); it++)
    {
        std::string newrole = Role(*it);
        builtMachine.addRole(newrole);
    }
}

} //end of namespace acl
} //end of namespace fipa
