/**
 *
 * \file statemachine_builder.h
 * \author Mircea Cretu Stancu
 * \brief defines the StateMachineBuilder Class, used to generate a state macine from a spec. file
 * 
 * \version 1.0
 */

#ifndef _FIPAACL_CONVMONITOR_STATEMACHINEBUILDER_H_
#define _FIPAACL_CONVMONITOR_STATEMACHINEBUILDER_H_

#include <fipa_acl/conversation_monitor/statemachine.h>
#include <fipa_acl/conversation_monitor/state.h>
#include <tinyxml/tinyxml.h>
#include <tinyxml/tinystr.h>
#include <vector>

namespace fipa {
    
namespace acl {

/**
    \class StateMachineBuilder
    \brief implements a XML parser that generates a state machine from a given spec. file; uses the tinyXML library
*/
class StateMachineBuilder {
        
    private:
        /// \param roles: vector of all the identified roles from the spec. file
        std::vector<std::string> roles;
        
        /// \param states: vector of all the states' ids identified in the spec. file; needed for the transitions parsing
        std::vector<std::string> states;
        
        /// \param initialState: the initial state parameter of the state machine
        std::string initialState;
        
        /// \param builtMachine: the state machine resulted from the last spec. file
        StateMachine* builtMachine;

        static std::string resourceDir;
        
    public:
        /// \brief node attribute names used in the spec standard; most general ones such as "state", "transition" are hard-implement
        static const std::string from;
        static const std::string to;
        static const std::string target;
        static const std::string id;
        static const std::string final;
        static const std::string performative;
        static const std::string initial;
    
    public:
        /**
	  \brief default constructor; initializes class fields
        */
        StateMachineBuilder();

        /**
        * Set the resource dir where to search for the protocol definitions
        */
        static void setProtocolResourceDir(const std::string& resourceDir);
        
        /**
	   after building the state machine from the spec. file calls the methods that do the necessary implicit additional adjustments
	  \param file name of file with the specification to be parsed
	   the ready-to use state machine(builtMachine)
          \param file
        */
        StateMachine* getFunctionalStateMachine(const std::string& file);
        
        /**
	   populates the builtMachine parameter of the class based on the given spec. file
	  \param file name of the file with the specification
	  \return builtMachine, BUT: raw, without the implicit parameters set(default states; default transitions)
        */
        StateMachine* loadSpecification(const std::string& file);
        
        /**
	   Identical to loadSpecification but does not return the state machine(i.e: it just populates the builtMachine param)
	  \param file name of the file with the specification
        */
        void locallyLoadSpecification(const std::string& file);
        
        /**
	   root function in the tree of function calls that populate the builtMachine param; it parses the root element of the file
		which should be <scxml>
	  \param sm the element describing the stateMachine
        */
        void parseStateMachineNode(TiXmlElement *sm);
        
        /**
	  \brief method that parses the <state> element of the spec. file
	  \param st
        */
        State parseStateNode(TiXmlElement *st);
        
        /**
	  \brief  recursive method that does preliminary parsing of <state> elements(i.e: it adds their names to the states field);
		needed so that the targets of the transitions can be checked for correctness when the transitions are parsed;
		calls the parseStateNode method
	  \param st
        */
        void addStates(TiXmlElement *st);
        
        /**
	  \brief method that parses <transition> element of the spec. file
	  \param trans
	  \return fipa::acl::Transition with string values populated based on the parameters of the spec file
        */
        Transition parseTransitionNode(TiXmlElement *trans);
        
        /**
	  \brief method that adds the generic names of the involved agents detected in the spec file to the builtMachine field
        */
        void addInvolvedAgentsMap();
};


} //end of acl
} //end of fipa

#endif
