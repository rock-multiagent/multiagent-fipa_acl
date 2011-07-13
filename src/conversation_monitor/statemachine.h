/**
 *
 * \file statemachine.h
 * \author Mircea Cretu Stancu
 * \brief describes the structure and operation of a state machine
 * 
 * \version 1.0
 */

#ifndef _FIPAACL_CONVMONITOR_STATEMACHINE_H_
#define _FIPAACL_CONVMONITOR_STATEMACHINE_H_

#include <fipa_acl/conversation_monitor/types.h>
#include <fipa_acl/bitefficient_message.h>
#include <vector>

namespace fipa {
namespace acl {

    
/**
* \brief forward declaration of State class to reslve the circular dependency between these two classes 
*/
class State;

class StateMachineBuilder;

/**
* \class StateMachine
* \brief The front end class of the library; implements the interaction protocol rules, processes the flow of messages and checks
* for compliance with the defined protocol; it is designed for this sole purpose(not a general state machine)
*/
class StateMachine 
{

private:

    /** \param states stl vector of the states built for the implementation of the protocol */
    std::vector<State> states;
    
    /** \param involvedAgents record of the generic agent roles involved in the protocol as well as what agents play which role 
    	once they have been assigned their roles
    */
    std::vector<AgentMapping> involvedAgents;
    
    /**
      \param preImposedRoles roles that need to be correlated to a master StateMachine(i.e: only useful if the current StateMachine
     is a sub-protocol to another StateMachine)
    */
    std::vector<RoleCorrelation> preImposedRoles;
    
    /** \param owner AgentID, owner of the state machine; determines the perspective from which the roles are assigned 
      NOTE: no symbolic value for "owner not set" case defined yet 
    */
    AgentID owner;
    
    /** \param currentState pointer to the current state of the state machine(from the states vector) */
    std::string currentState;
    
    /** \param active flag variable to indicate whether a conversation has been successfully initiated(i.e: wit a valid message) */
    bool active;
    
    /** \param conversationOver flag variable to indicate whether the confersation has reached a final state(= is over) */
    bool conversationOver;

    /**
    * Flag to define whether the statemachine is valid and thus usable
    */
    bool isValidStateMachine;

    /**
      \param cancelMetaP (i.e.: cancel meta protocol) implements the fipa defined "meta conversation" that should be available on top
    		  of every other interaction protocol; through it, the initiator of a conversation can terminate the 
    		  conversation early(yet validly) with any of its interlocutors
      NOTE: Not yet completely functional(i.e: need to modify it to remove the interlocutor from the list)
    */
    std::vector<StateMachine> cancelMetaP;
    
    /** \param ontology global characteristic of the conversation initialized from the first alid, processed message */
    Ontology ontology;
    /** \param language global characteristic of the conversation initialized from the first alid, processed message */
    Language language;
    /** \param protocol global characteristic of the conversation initialized from the first alid, processed message */
    Protocol protocol;
    /** \param convid global characteristic of the conversation initialized from the first alid, processed message */
    ConversationID convid;
    /** \param encoding global characteristic of the conversation initialized from the first alid, processed message */
    Encoding encoding;
    //ReplyBy replyBy;
    //ReplyWith replyWith;
    
    /**
      \class Transition: declared as friend because it accesses the global variables and operates on the state machine (i.e.: changes current state,etc) often
    */
    friend class Transition;
    /// StateMachineTest class declared as friend for testing purposes(NOT for the final version)
    friend class StateMachineTest;
    
    friend class StateMachineBuilder;
    
public:
    /**
      \brief static names used as constant defaults for state names and role names
    */
    static const std::string NOT_UNDERSTOOD;
    static const std::string CONV_CANCELLED;
    static const std::string INITIAL;
    static const std::string INITIATOR;
    
    /** \brief emtpy constructor, initializes some fields*/
    StateMachine();

    /** \brief constructor that initializes the owner of the machine as well
      \param _owner: the intended owner of the state machine
    */
    StateMachine(const AgentID& _owner);
    
    StateMachine(const StateMachine&);

    ~StateMachine();

    /**
    * Copy assignment
    */
    StateMachine& operator=(const StateMachine& target);
    
    /** \brief setter method for the initial state of the state machine(takes the name-uid- of the state as parameter)*/
    bool setInitialState(const std::string&);

    /** \brief setter method for the owner of the machine */
    bool setOwner(const AgentID&);

    /** \brief method to be called with the first message from the message flow as parameter; it initializes fields 
    	and starts the conversation
      \return 0 if machine started successfully; != otherwise(currently only 1 as error code - more to be added when needed)
    */
    int startMachine(const ACLMessage& msg);

    /**
      \brief method to be called to process every message of the message flow(except the first one)
      \return 0 if successful; 1 otherwise
    */
    int consumeMessage(const ACLMessage& msg);
    //int initializeMachineFields(ACLMessage msg);
    
    
    /** \brief getter method for a state from the states vector
      \param name of the state to be searched as a strig
      \return pointer to the coresponding needed state
    */
    State* getStateByName(const std::string& name);

    /** \brief getter method for a state from the states vector
      \param state the needed state (as object; by value)
      \return pointer to the coresponding needed state
    */
    State* getStateByName(State state) const;

    /** \brief method that generates the implicit states(sometimes not mentioned in the IP -- i.e: not-understood state) */
    void generateDefaultStates();

    /** \brief method that generates implicit transitions (sometimes not mentioned int the IP -- i.e: not-understood transitions) */
    void generateDefaultTransitions();

    /**
      \brief method that adds a state to the states vector of the state machine
      \param state the state intended to be added
      \return true if successful(i.e: unique among the other states); false otherwise
    */
    bool addState(State& state);

    /** \brief method to check whether the conversation has reached a final state(= is over)
      \return value of conversationOver
    */
    bool isConversationOver() const;

    /** \brief method to check whether the conversation has been properly started
      \return value of active variable
    */
    bool isActive() const;

    /** \brief method to add a generic role to the involvedAgents field
      \param myrole name of the intended role given as string
    */
    void addRole(const Role& myrole);

    /** \brief method to assign a generic role to a speciffic agent
      \param myrole string; name of the role intended to be set
      \param myagent AgentID; the intended agent
      \return true if successfully added; false otherwise
    */
    bool setRole(const Role& myrole,const AgentID& myagent);

    /** \brief method to assign a generic role to a vector of agents
      \param myrole string; name of the role intended to be set
      \param agents std::vector< AgentID >; the intended vector of agents
      \return true if successfully added; false otherwise
    */
    bool setRole(const Role& myrole,const std::vector<AgentID>& agents);

        /** \brief method to check whether a speciffic role exists in the involvedAgents field
	  \param myrole string; the intended to be checked role
	  \return true if role exists; false otherwise
        */
        bool checkIfRoleExists(const Role& myrole);

        /** 
          \brief method to check what role a speciffic agents is assigned to
	  \param ag agent whose role is to be checked
	  \return the assigned role; empty string if agent is not assigned
        */
        Role getAgentRole(const AgentID& ag);

        /**
          \brief method to check whether a certain role has an agent assigned to it
	  \param role role to be checked
	  \return true if role is set; false otherwise
        */
        bool checkIfAgentAssigned(const AgentID& role);
        
        /**
	  \brief returns a vector with all the agents(AgentID) assigned to a specific role
	  \param myrole Role  that we want to retrieve assigned agents forward
	  \return myrole std::vector<AgentID> populated with the coresponding AgentIDs(empty if role not set yet)
	  
        */
        std::vector<AgentID> getAgentsAssignedTo(Role& myrole) const;
        
        /**
	  \brief returns the vector of RoleCorrelation which keeps track of roles that need to be synchronized with a master StateMachine
        */
        std::vector<RoleCorrelation> getRoleCorrelation() const;
        
        /**
	  \brief add a new pair of roles that needs to be correlated(between this StateMachine and a master StateMachine);
		!this method need to be called after involvedAgents field has been populated with all the generic names(roles) of the 
		involved agents
	  \param mymaster role from the master StateMachine
	  \param myresident own role(must be registerred in the involvedAgents field)
        */
        void addRoleCorrelation(const Role& mymaster,const Role& myresident);
        
        bool checkIfRoleSet(const Role&);
        //void removeInterlocutor(AgentID);
        //void removeInterlocutor(std::vector<AgentID>);
        /** \brief method that sends an update signal downwards towards transition level, so that the latter update the agents set for
		each role
            method that sends a signal down to transition level so that the latter update their agent fields such expectedSenders,etc), based on the fields of the stateMachine
        */
        void updateAllAgentRoles();
        
        /**
	  \brief overload to updateAllAgentRoles() that takes into account roles that need to be correlated with a master StateMachine
	  \param myrole role that needs to be correlated to the role of a master StateMachine
	  \param myagents vector of agents that are assigned in the master StateMachine to the myrole Role and need to be correlated
        */
        void updateAllAgentRoles(const Role& myrole, const std::vector<AgentID>& myagents);
        
        /**
        * Check is statemachine is valid, i.e. transitions and states are consistent
        */
        bool isValid() const;
        
        /**
	  \brief misc method added to easily visualize a built stateMachine;
        */
        void print();

        /**
        * Resolve the current state 
        */
        State* getCurrentState();
        
        Language 			getLanguage() const;
        Ontology 			getOntology() const;
        Encoding 			getEncoding() const;
        Protocol 			getProtocol() const;
        std::vector<StateMachine> 	getCancelMetaP() const;
        std::vector<AgentMapping> 	getInvolvedAgents () const;
        AgentID 			getOwner() const;
        std::vector<State> 		getStates() const;
        ConversationID		getConversationID() const;

        /**
        * Validate the state machine that all existing transaction and from to field have a target
        */
        void validate(bool associatedMachine = false);
        
    private:
        /** \brief method called by the constructors that initializes some fields */
        void initializeObjectFields();
        /** \brief method that removes an agent from the involvedAgents field once the owner of the machine has finnished the 
		conversation with it
	  \param ag AgentID; agent to be removed
        */
        void removeInterlocutor(const AgentID& ag);
        /** \brief method that removes a vector of agents from the involvedAgents field once the owner of the machine has finnished the 
		conversation with them
	  \param ag std::vector< AgentID >; agents to be removed
        */
        void removeInterlocutor(const std::vector<AgentID>& agents);
        /** \brief method that sends a signal down to transition level so that the latter initialize their parametrically defined fields
		(such as from, to, nextStateName), based on the fields of the stateMachine
        */
        void loadParameters();

        /** \brief method that creates a cancel meta protocol and adds it to the cancelMetaP vector, based on a "cancel" message that is
		received
	  \param msg ACLMessage; message from which to build the new cancel meta protocol
	  \return 0 if successful; 1 otherwise
        */
        int createAndStartNewCancelMetaProtocol(const ACLMessage &msg);
        /** \brief helper method for the createAndStartCancelMetaProtocol method;
	  \param string; generic role among the ones involved in the conversation with which the cancel protocol is initialized
	  \return StateMachine; the created cancel meta protocol
        */
        StateMachine generateCancelMetaProtocol(Role);

};


} // end of acl
} // end of fipa

#endif
