/**
 *
 * \file message_generator/test.cpp
 * \author Mircea Cretu Stancu
 * \brief main program made for the sole purpose of testing the functionality of the acl module - bit efficient encoding
 * 
 * \version 1.0

 */
#include <iostream>
#include <set>
#include <iterator>
#include <string>
#include <cstring>
#include "acl_message_output_parser.h"

#include <fipa_acl/message_parser/message_parser.h>



using namespace fipa::acl;
using namespace std;

void printAgentID(AgentID *agent);
void printAgentIDset(set<AgentID*>* myset);
void printUserdefParamset(set<UserdefParam*>* params);

void printMessage(ACLMessage *msg)
{
    cout<<"=================================Printing Message=================================\n";
    cout<<"performative:\t"<< msg->getPerformative()<<endl;
    
    if (!(*msg).getContent().empty()) cout<<"content:\t"<< msg->getContent()<<endl;
    if (!(*msg).getReplyWith().empty()) cout<<"reply with:\t"<< msg->getReplyWith()<<endl;
    if (!msg->getReplyBy().isNull()) cout<<"reply by:\t"<< msg->getReplyByString(true)<<endl;
    if (!(*msg).getInReplyTo().empty()) cout<<"in reply to:\t"<< msg->getInReplyTo()<<endl;
    if (!(*msg).getLanguage().empty()) cout<<"language:\t"<< msg->getLanguage()<<endl;
    if (!(*msg).getEncoding().empty()) cout<<"encoding:\t"<< msg->getEncoding()<<endl;
    if (!(*msg).getOntology().empty()) cout<<"ontology:\t"<< msg->getOntology()<<endl;
    if (!(*msg).getProtocol().empty()) cout<<"protocol:\t"<< msg->getProtocol()<<endl;
    if (!(*msg).getConversationID().empty()) cout<<"conversation id:\t"<< msg->getConversationID()<<endl;
    if ((*msg).getSender() != NULL) {cout<<"sender:\n"; printAgentID(msg->getSender());}
    if (!((*msg).getAllReceivers()->empty())) { cout<<"receivers:\n"; printAgentIDset(msg->getAllReceivers());}
    if (!(*msg).getAllReplyTo()->empty()) {cout<<"reply to:\n"; printAgentIDset(msg->getAllReplyTo());}
    if (!(*msg).getUserdefParams()->empty()) printUserdefParamset(msg->getUserdefParams());

}

void printAgentIDset(set<AgentID*>* myset)
{
    cout<<"\t==================Agent AID set==================\n";
    set<AgentID*>::iterator it = myset->begin();
    for(it; it != myset->end(); ++it)
        printAgentID(*it);
}
void printAgentID(AgentID *agent)
{
    cout<<"\t==================Agent AID==================\n";
    if (!agent->getName().empty()) cout<<"\t\tname:\t"<< agent->getName()<<endl;
    if (!agent->getAddresses()->empty())
    {
        cout<<"\t\taddresses:\t\n";
        set<string>::iterator it = agent->getAddresses()->begin();
        for(it; it != agent->getAddresses()->end(); ++it)
	  cout<<"\t\t\t"<<*it<<endl;
    }
    if (!agent->getResolvers()->empty()) {cout<<"\t\tresolvers:\t\n"; printAgentIDset(agent->getResolvers());}
    if (!agent->getUserdefParams()->empty()) {cout<<"\t\tUser Defined Parameters:\t\n";printUserdefParamset(agent->getUserdefParams());}
}

void printUserdefParamset(set<UserdefParam*>* params)
{
    cout<<"\t\t==================User Defined Parameters==================\n";
    set<UserdefParam*>::iterator it = params->begin();
    for(it; it != params->end(); ++it)
    {
        cout<<"\t\tparam name:\t"<< (*(*it)).getName()<<endl;
        cout<<"\t\tparam value:\t"<< (*(*it)).getValue()<<"\n\n";
    }
}


int main(int argc, char** argv)
{

	fipa::acl::ACLMessage *testmessage = new fipa::acl::ACLMessage(std::string("query-ref"));

        fipa::acl::AgentID *sender = new fipa::acl::AgentID(std::string("modulename"));
        testmessage->setSender(sender);

        fipa::acl::AgentID *receiver = new fipa::acl::AgentID(std::string("moduleD"));
        testmessage->addReceiver(receiver);

        fipa::acl::ACLMessageOutputParser testparser = fipa::acl::ACLMessageOutputParser();
        testparser.setMessage(testmessage);

        std::string received = testparser.getBitMessage();


        MessageParser parser = MessageParser();
        fipa::acl::ACLMessage* aclmessage = parser.parseData(received);
        printMessage(aclmessage);
    return 0;
}


