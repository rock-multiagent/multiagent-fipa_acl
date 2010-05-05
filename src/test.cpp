/**
 *
 * \file test.cpp
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
//#include <conio.h>
#include "ACLMessage.h"
#include "ACLMessageOutputParser.h"
#include "UserdefParam.h"

//#include "../../message-parser/src/message_parser.h"


using namespace fipa::acl;
using namespace std;

void printAgentAID(AgentAID *agent);
void printAgentAIDset(set<AgentAID*>* myset);
void printUserdefParamset(set<UserdefParam*>* params);

void printMessage(ACLMessage *msg)
{
    cout<<"=================================Printing Message=================================\n";
    cout<<"performative:\t"<< msg->getPerformative()<<endl;
    
    if (!(*msg).getContent().empty()) cout<<"content:\t"<< msg->getContent()<<endl;
    if (!(*msg).getReplyWith().empty()) cout<<"reply with:\t"<< msg->getReplyWith()<<endl;
    if (!(*msg).getReplyBy1().empty()) cout<<"reply by1:\t"<< msg->getReplyBy1()<<endl;
    if (!(*msg).getInReplyTo().empty()) cout<<"in reply to:\t"<< msg->getInReplyTo()<<endl;
    if (!(*msg).getLanguage().empty()) cout<<"language:\t"<< msg->getLanguage()<<endl;
    if (!(*msg).getEncoding().empty()) cout<<"encoding:\t"<< msg->getEncoding()<<endl;
    if (!(*msg).getOntology().empty()) cout<<"ontology:\t"<< msg->getOntology()<<endl;
    if (!(*msg).getProtocol().empty()) cout<<"protocol:\t"<< msg->getProtocol()<<endl;
    if (!(*msg).getConversationID().empty()) cout<<"conversation id:\t"<< msg->getConversationID()<<endl;
    if ((*msg).getSender() != NULL) {cout<<"sender:\n"; printAgentAID(msg->getSender());}
    if (!((*msg).getAllReceivers()->empty())) { cout<<"receivers:\n"; printAgentAIDset(msg->getAllReceivers());}
    if (!(*msg).getAllReplyTo()->empty()) {cout<<"reply to:\n"; printAgentAIDset(msg->getAllReplyTo());}
    if (!(*msg).getUserdefParams()->empty()) printUserdefParamset(msg->getUserdefParams());

}

void printAgentAIDset(set<AgentAID*>* myset)
{
    cout<<"\t==================Agent AID set==================\n";
    set<AgentAID*>::iterator it = myset->begin();
    for(it; it != myset->end(); it++)
        printAgentAID(*it);
}
void printAgentAID(AgentAID *agent)
{
    cout<<"\t==================Agent AID==================\n";
    if (!agent->getName().empty()) cout<<"\t\tname:\t"<< agent->getName()<<endl;
    if (!agent->getAdresses()->empty())
    {
        cout<<"\t\tadresses:\t\n";
        set<string>::iterator it = agent->getAdresses()->begin();
        for(it; it != agent->getAdresses()->end(); it++)
	  cout<<"\t\t\t"<<*it<<endl;
    }
    if (!agent->getResolvers()->empty()) {cout<<"\t\tresolvers:\t\n"; printAgentAIDset(agent->getResolvers());}
    if (!agent->getUserdefParams()->empty()) {cout<<"\t\tUser Defined Parameters:\t\n";printUserdefParamset(agent->getUserdefParams());}
}

void printUserdefParamset(set<UserdefParam*>* params)
{
    cout<<"\t\t==================User Defined Parameters==================\n";
    set<UserdefParam*>::iterator it = params->begin();
    for(it; it != params->end(); it++)
    {
        cout<<"\t\tparam name:\t"<< (*(*it)).getName()<<endl;
        cout<<"\t\tparam value:\t"<< (*(*it)).getValue()<<"\n\n";
    }
}


int main(int argc, char** argv)
{
//    ACLMessage* m = new ACLMessage();
//	(*m).setPerformative(ACLMessage::perfs[ACLMessage::REQUEST_WHENEVER]);
//	(*m).setConversationID(std::string("convid"));

// std::cout<<int(char(0xfa))<<std::endl<<int(char(0x0d))<<std::endl<<int(char(0xff))<<std::endl;

//ACLMessage* m2 = new ACLMessage(std::string("abcd"));

ACLMessage* m3 = new ACLMessage(ACLMessage::perfs[ACLMessage::REQUEST_WHENEVER]);

	
    //(*m3).setConversationID("plm");
    (*m3).setReplyBy1("20091201010101111");

	//UserdefParam *p = new UserdefParam();
//std::cout<<"1\n";
	//(*p).setName(std::string("cretzu")); 
	//(*p).setValue(std::string("mare"));
	//(*m3).addUserdefParam(p); 
	
	UserdefParam *p1 = new UserdefParam();

	(*p1).setName(std::string("cretzu")); 
	(*p1).setValue(std::string("marfa")); ;
	(*m3).addUserdefParam(p1); 
	
	
	AgentAID *a1 = new AgentAID(std::string("r1"));
	(*a1).addAdress(std::string("adr1"));
	(*a1).addAdress(std::string("adr2"));	
	(*m3).setSender(a1);
	(*a1).addUserdefParam(p1);
	
	AgentAID *a2 = new AgentAID(std::string("r2"));
	AgentAID *a3 = new AgentAID(std::string("r3"));
	AgentAID *a4 = new AgentAID(std::string("r4"));
	AgentAID *a5 = new AgentAID(std::string("r5"));
	AgentAID *a6 = new AgentAID(std::string("r6"));
	AgentAID *a7 = new AgentAID(std::string("r7"));
	UserdefParam *p4 = new UserdefParam();
	
	
	(*a6).addAdress(std::string("adr1"));
	(*a6).addAdress(std::string("adr2"));	
	(*m3).addReplyTo(a6);
	(*a6).addResolver(a7);
	(*a6).addResolver(a2);
	
	(*m3).addReceiver(a3); 
	
	(*a7).addAdress(std::string("adr1"));
	(*a7).addAdress(std::string("adr2"));	
	(*m3).addReplyTo(a7);
	(*a7).addResolver(a6);

	
	(*a1).addResolver(a2);	
	(*m3).addReceiver(a2); 
	
	(*a1).addResolver(a4);	
	(*m3).addReceiver(a4); 
	
	
	(*a2).addResolver(a5);	

	
	
	
	
	UserdefParam *p2 = new UserdefParam();
	(*p2).setName(std::string("cretzu")); 
	(*p2).setValue(std::string("marf")); ;
	(*a1).addUserdefParam(p2); 
	(*m3).addUserdefParam(p2);
	
	(*m3).addUserdefParam(p2);
	UserdefParam *p3 = new UserdefParam();
	(*p3).setName(std::string("stancu")); 
	(*p3).setValue(std::string("marf"));
	(*m3).addUserdefParam(p3); 
	
	(*p4).setName(std::string("cretzu")); 
	(*p4).setValue(std::string("marf"));
	(*a1).addUserdefParam(p4);

   
//ACLMessage* m4 = new ACLMessage(ACLMessage::perfs[ACLMessage::REQUEST_WHENEVER]);

(*m3).setProtocol(std::string("myprotocol")); 
(*m3).setLanguage(std::string("mylang")); 
(*m3).setReplyWith(std::string("myreplywith"));
(*m3).setInReplyTo(std::string("inreplyto"));
(*m3).setEncoding(std::string("encoding"));
(*m3).setOntology(std::string("myontology"));
(*m3).setContent(std::string("my_content"));
(*m3).setConversationID(std::string("convID"));


  //printm(*m);
    //getch();
    
	ACLMessageOutputParser a = ACLMessageOutputParser();
	a.setMessage(m3);
	std::string storage = a.getBitMessage();
	//std::cout<<storage<<std::endl<<a.getBitMessage().length()<<std::endl;
	//std::cout<<a.getBitHeader()<<std::endl;
	//std::cout<<a.getBitMessageType()<<std::endl;
	//std::cout<<int('\0')<<0<<std::endl;
	//	std::cout<<char(21)<<"\t"<<int(a.getBitMessage()[3])<<std::endl;
	
	//if (!a.printParsedMessage(std::string("TestMessage26.txt")))
	//	return 1;

delete m3;
delete p1;
delete a1;
delete a2;
delete a3;
delete a4;
delete a5;
delete a6;
delete a7;
//delete p3;  
//delete p;

//delete p2;
  

        //std::cout<<"written\n";
  
  	//std::string storage = a.getBitMessage();
	//std::cout<<"storage_set\n";
	//MessageParser rebuilder = MessageParser();
	//std::cout<<"rebuilder created\n";
	//ACLMessage* rebuilt = rebuilder.parseData(storage);
	
	//if (!rebuilt); //std::cout <<"null message returned\n";
	//printMessage(rebuilt);
 /* 
   
  	typedef fipa::acl::bitefficient_grammar<std::string::const_iterator> bitefficient_grammar;
	bitefficient_grammar grammar;
	fipa::acl::Message parseTree;

	using boost::spirit::ascii::space;
	std::string::const_iterator iter = storage.begin();
	std::string::const_iterator end = storage.end();
	bool r = phrase_parse(iter, end, grammar, space, parseTree);

	if(r && iter == end)
	{
	ACLMessage *rebuilt;
	//MessageRebuilder rebuilder = MessageRebuilder(); 
	//rebuilt = rebuilder.buildMessage(parseTree);
  	}
  */
  
    //std::cout<<(*rebuilt).getContent().length()<<std::endl;
    //std::cout<<(*rebuilt).getReplyWith()<<std::endl;
  
  
    return 0;
}
 /*               
}

void parseBitEfficient(std::string stream)
{
     std::fstream out(stream, std::ios::out | std::ios::binary);
	std::string output = getBitMessage();
	for (int i = 0; i < output.length(); i++)
		out.put(output[i]);

	out.close();
     
     
}
*/

