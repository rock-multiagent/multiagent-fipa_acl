#include "StateMachineBuilder.h"

#include <iostream>
#include <algorithm>
#include <cstring>

namespace fipa {
namespace acl {

const std::string StateMachineBuilder::from = 		std::string("from");
const std::string StateMachineBuilder::to = 		std::string("to");
const std::string StateMachineBuilder::target = 		std::string("target");
const std::string StateMachineBuilder::id = 		std::string("id");
const std::string StateMachineBuilder::final = 		std::string("final");
const std::string StateMachineBuilder::performative = 	std::string("performative");
const std::string StateMachineBuilder::initial = 		std::string("initial");

StateMachineBuilder::StateMachineBuilder()
{
    roles.clear();
    states.clear();
    initialState.clear();
}

StateMachine StateMachineBuilder::getFunctionalStateMachine(std::string infile)
{
    loadSpecification(infile);
    
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
        std::cout<<"error loading the spec file\n";
        exit(1);
    }
    builtMachine = StateMachine();
    std::cout<<"@file opened\n";
    TiXmlElement *sm = file.RootElement();
    parseStateMachineNode(sm);
    
    return builtMachine;
}

void StateMachineBuilder::parseStateMachineNode(TiXmlElement *sm)
{
    const char *value;
    
    value = sm->Attribute(StateMachineBuilder::initial.c_str());
    std::cout<<"@retrieved value of the \"initial\" parameter\n";
    if (value)
        initialState = std::string(value);
    else;
        //TODO throw some exception here
        
    std::cout<<"@saved the initial state of the machine\n";
    
    TiXmlHandle handleSm = TiXmlHandle(sm);
    TiXmlElement* child = handleSm.FirstChild( "state" ).ToElement();
    
    if (child)
    std::cout<<"@found first state element\n";
    else std::cout<<"!!!state element is NULL";
    
    addStates(child);
    builtMachine.setInitialState(initialState);
    addInvolvedAgentsMap();
    
}

State StateMachineBuilder::parseStateNode(TiXmlElement *st)
{
    State ret;
    std::cout<<"\t@parseStateNode method\n";
    
    const char *value;
    value = st->Attribute(StateMachineBuilder::id.c_str());
    //if (value) ---- should not be necessary to check because it is already checked in the caller function(parseStates())
        ret.setUID(std::string(value));
        std::cout<<"\t\t#uid set\t"<<value<<"\n";
    //else;
        //TODO: throw some exception here
    
    value = NULL;
    value = st->Attribute(StateMachineBuilder::final.c_str());
    std::cout<<"\t\t#value field retrieved for final:\n"; //<< value <<"\n";
    if (value)
    {
        if (!strcmp(value,"yes"))
	  ret.setFinal(true);
        else
	  if (!strcmp(value,"no"))
	      ret.setFinal(false);
	  else;
	      //TODO throw some exception here
    }
    std::cout<<"\t\t#set the final variable\n";
        
    ret.setOwningMachine(&builtMachine);
    
    TiXmlHandle handleState = TiXmlHandle(st);
    TiXmlElement *trans = handleState.FirstChildElement("transition").ToElement();
    for (trans; trans != NULL; trans = trans->NextSiblingElement("transition") )
    {
        std::cout<<"\t\t!!!trans for\n";
        Transition t = Transition(parseTransitionNode(trans));
        std::cout<<"\t\t!!!returned\n";
        std::cout<<"\t\t&&&set parameter from: "<<t.getFrom()<<"\n";
        std::cout<<"\t\t&&&set parameter to: "<<t.getTo()<<"\n";
        std::cout<<"\t\t&&&set parameter next state: "<<t.getNextStateName()<<"\n";
        std::cout<<"\t\t&&&set parameter performative: "<<t.getExpectedPerformative()<<"\n";
        t.setOwningState(&ret);
        ret.addTransition(t);
        
        std::cout<<"\t\t!!!end of one for loop\n";
    }
    
    //TODO: implement specification for subprotocols
    return ret;
}
void StateMachineBuilder::addStates(TiXmlElement *st)
{
    std::cout<<"@addStates method\n";
    TiXmlElement *next;
    const char *value = NULL;
    value = st->Attribute(StateMachineBuilder::id.c_str());
    if (value)
    {
        std::cout<<"\t#retrieved id of the state\n";
        std::vector<std::string>::iterator it = find(states.begin(), states.end(), std::string(value));
        if ( it == states.end() )
	  states.push_back(std::string(value));
        else;
	  //TODO: throw some exception here(duplicate state)
    }
    else;
        //TODO: throw some exception here
    
    if ( (next = st->NextSiblingElement("state")) != NULL )
    {
        addStates(next);
        builtMachine.addState(parseStateNode(st));        
    }
    else
        builtMachine.addState(parseStateNode(st));
}

Transition StateMachineBuilder::parseTransitionNode(TiXmlElement *trans)
{
    Transition ret;
    const char *value = NULL;
    std::cout<<"\t@parseTransitionNode method\n";
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
        else;
	  //TODO: throw some exception here(no such state in the spec file)
    }
    else;
        //TODO: throw some exception here

    value = NULL;
    value = trans->Attribute(StateMachineBuilder::performative.c_str());
    if (value)
    {
        ret.setExpectedPerformative(std::string(value));
    }
    else;
        //TODO: throw some exception here
    
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