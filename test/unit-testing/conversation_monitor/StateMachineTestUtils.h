/** \file: StateMachineTest.cpp
 *
 * \author: Mircea Cretu Stancu
 *
 * version: 1.0
 */
#include <iostream>
#include <fstream>
#include "StateMachineTest.h"

namespace fipa { namespace acl {

std::vector<ACLMessage> buildRequestMessageFlow(AgentID sender, AgentID receiver);
void printMessage( ACLMessage &msg);
void printUserdefParamset( std::vector<UserdefParam> &params);
void printAgentID( AgentID &agent);
void printAgentIDset( std::vector<AgentID> &myset);

std::vector<ACLMessage> buildRequestMessageFlow(AgentID sender, AgentID receiver)
{
    std::vector<ACLMessage> flow;
    flow.clear();
    ACLMessage m1 = ACLMessage(REQUEST);
    //m1.setPerformative(std::string("test performative"));
    m1.setLanguage(std::string("test language"));
    m1.setContent(std::string("test content"));
    m1.setEncoding(std::string("test encoding"));
    m1.setOntology(std::string("test ontology"));
    m1.setReplyWith(std::string("test reply_with"));
    m1.setReplyBy1(std::string("2010-12-23T12:00:37:980"));
    m1.setInReplyTo(std::string("test in_reply_to"));
    m1.setConversationID(std::string("test conversationID"));
    m1.setProtocol(std::string("test protocol"));
    m1.setSender(sender);
    m1.addReceiver(receiver);
    
    flow.push_back(m1);
    m1.setPerformative(ACLMessage::perfs[ACLMessage::AGREE]);
    m1.setLanguage(std::string("test language"));
    m1.setContent(std::string("test content"));
    m1.setEncoding(std::string("test encoding"));
    m1.setOntology(std::string("test ontology"));
    m1.setReplyWith(std::string("test reply_with"));
    m1.setReplyBy1(std::string("2010-12-23T12:00:37:980"));
    m1.setInReplyTo(std::string("test reply_with"));
    m1.setConversationID(std::string("test conversationID"));
    m1.setProtocol(std::string("test protocol"));
    m1.setSender(receiver);
    m1.deleteReceiver(receiver);
    m1.addReceiver(sender);
    flow.push_back(m1);
    
    m1.setPerformative(ACLMessage::perfs[ACLMessage::INFORM]);
    m1.setLanguage(std::string("test language"));
    m1.setContent(std::string("test content"));
    m1.setEncoding(std::string("test encoding"));
    m1.setOntology(std::string("test ontology"));
    m1.setReplyWith(std::string("test reply_with"));
    m1.setReplyBy1(std::string("2010-12-23T12:00:37:980"));
    std::string inrepto = std::string();
    inrepto.clear();
    m1.setInReplyTo(inrepto);
    m1.setConversationID(std::string("test conversationID"));
    m1.setProtocol(std::string("test protocol"));
    m1.setSender(receiver);
    flow.push_back(m1);
    
    std::cout<<"flow of messages built..\n";
    
    return flow;
}


void printMessage( ACLMessage &msg)
{
    std::cout<<"=================================Printing Message=================================\n";
    std::cout<<"performative:\t"<< msg.getPerformative()<<std::endl;
    
    if (!msg.getContent().empty()) 
        std::cout<<"content:\t"<< msg.getContent()<<std::endl; 
    if (!msg.getReplyWith().empty()) 
        std::cout<<"reply with:\t"<< msg.getReplyWith()<<std::endl;
    //if (!msg.getReplyBy1().empty()) 
        std::cout<<"reply by1:\t"<< msg.getReplyBy1(1)<<std::endl;
    if (!msg.getInReplyTo().empty()) 
        std::cout<<"in reply to:\t"<< msg.getInReplyTo()<<std::endl;
    if (!msg.getLanguage().empty()) 
        std::cout<<"language:\t"<< msg.getLanguage()<<std::endl;
    if (!msg.getEncoding().empty()) 
        std::cout<<"encoding:\t"<< msg.getEncoding()<<std::endl;
    if (!msg.getOntology().empty()) 
        std::cout<<"ontology:\t"<< msg.getOntology()<<std::endl; 
    if (!msg.getProtocol().empty()) 
        std::cout<<"protocol:\t"<< msg.getProtocol()<<std::endl;
    if (!msg.getConversationID().empty()) 
        std::cout<<"conversation id:\t"<< msg.getConversationID()<<std::endl;
    std::cout<<"sender:\n";
    AgentID aid = msg.getSender();
    printAgentID(aid); 
    if (!(msg.getAllReceivers().empty())) 
				  { std::cout<<"receivers:\n"; 
				    std::vector<AgentID> vec = msg.getAllReceivers();
				    printAgentIDset(vec);}
    if (!msg.getAllReplyTo().empty()) 
			        { std::cout<<"reply to:\n"; 
				std::vector<AgentID> vec = msg.getAllReplyTo();
				printAgentIDset(vec);}
    if (!msg.getUserdefParams().empty()) 
				 {  std::vector<UserdefParam> params = msg.getUserdefParams();
				    printUserdefParamset(params); }

}

void printAgentIDset( std::vector<AgentID> &myset)
{
    std::cout<<"\t==================Agent AID set==================\n";
    std::vector<AgentID>::iterator it = myset.begin();
    for(it; it != myset.end(); it++)
        printAgentID(*it);
}
void printAgentID( AgentID &agent)
{
    std::cout<<"\t==================Agent AID==================\n";
    if (!agent.getName().empty()) std::cout<<"\t\tname:\t"<< agent.getName()<<std::endl;
    if (!(agent.getAddresses().empty()))
    {
        std::cout<<"\t\tadresses:\t\n";
        std::vector<std::string> addr = agent.getAddresses();
        std::vector<std::string>::iterator it = addr.begin();
        for(it; it != addr.end(); it++)
	  std::cout<<"\t\t\t"<< *it<<std::endl;
    }
    if (!(agent.getResolvers().empty())) {std::cout<<"\t\tresolvers:\t\n"; 
				std::vector<AgentID> vec = agent.getResolvers();
				printAgentIDset(vec);}
    if (!(agent.getUserdefParams().empty())) {std::cout<<"\t\tUser Defined Parameters:\t\n";
				    std::vector<UserdefParam> params = agent.getUserdefParams();
				    printUserdefParamset(params);}
}

void printUserdefParamset( std::vector<UserdefParam> &params)
{
    std::cout<<"\t\t==================User Defined Parameters==================\n";
    std::vector<UserdefParam>::iterator it = params.begin();
    for(it; it != params.end(); it++)
    {
        std::cout<<"\t\tparam name:\t"<< (*it).getName()<<std::endl;
        std::cout<<"\t\tparam value:\t"<< (*it).getValue()<<"\n\n";
    }
}

}}
