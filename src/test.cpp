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

using namespace fipa::acl;



void printm(ACLMessage m){
     std::cout<<m.getPerformative()<<std::endl<<m.getReplyBy()<<std::endl<<m.getConversationID()<<std::endl;    
/*	
	std::set<UserdefParam*>* s = new std::set<UserdefParam*>();
	//s = m.getUserdefParams();
std::cout<<"check first\n";
	std::set<UserdefParam*>::iterator it = (*s).begin();
std::cout<<"check\n";
	for (it; it!=(*s).end();it++)
		std::cout<<(*(*it)).getName()<<"\t"<<(*(*it)).getValue()<<std::endl;
*/
}


int main(int argc, char** argv)
{
//    ACLMessage* m = new ACLMessage();
//	(*m).setPerformative(ACLMessage::perfs[ACLMessage::REQUEST_WHENEVER]);
//	(*m).setConversationID(std::string("convid"));

// std::cout<<int(char(0xfa))<<std::endl<<int(char(0x0d))<<std::endl<<int(char(0xff))<<std::endl;

//ACLMessage* m2 = new ACLMessage(std::string("abcd"));

ACLMessage* m3 = new ACLMessage(ACLMessage::perfs[ACLMessage::REQUEST_WHENEVER]);

	
    //(*m).setConversationID("plm");
    //(*m3).setReplyBy1("20091201010101111");
/*
	UserdefParam *p = new UserdefParam();
//std::cout<<"1\n";
	(*p).setName(std::string("cretzu")); 
	(*p).setValue(std::string("mare"));
	(*m3).addUserdefParam(p); 
	
	UserdefParam *p1 = new UserdefParam();

	(*p1).setName(std::string("cretzu")); 
	(*p1).setValue(std::string("marf")); ;
	(*m3).addUserdefParam(p1); 


	AgentAID *a1 = new AgentAID(std::string("r1"));
	(*a1).addAdress(std::string("adr1"));
	(*a1).addAdress(std::string("adr2"));	
	(*m3).setSender(a1);

	AgentAID *a2 = new AgentAID(std::string("r2"));
	(*a1).addResolver(a2);	
	(*m3).addReceiver(a2); 
	AgentAID *a4 = new AgentAID(std::string("r4"));
	(*a1).addResolver(a4);	
	(*m3).addReceiver(a4); 
	
	AgentAID *a5 = new AgentAID(std::string("r5"));
	(*a2).addResolver(a5);	

	
	AgentAID *a3 = new AgentAID(std::string("r3"));
	(*m3).addReceiver(a3); 
	
	UserdefParam *p1 = new UserdefParam();
	(*p1).setName(std::string("cretzu")); 
	(*p1).setValue(std::string("marf")); ;
	(*a1).addUserdefParam(p1); 
*/
   
ACLMessage* m4 = new ACLMessage(ACLMessage::perfs[ACLMessage::REQUEST_WHENEVER]);
(*m4).setProtocol(std::string("myprotocol"));   
  //printm(*m);
    //getch();
	ACLMessageOutputParser a = ACLMessageOutputParser();
	a.setMessage(m4);
	std::cout<<a.getBitMessage()<<std::endl<<a.getBitMessage().length()<<std::endl;
	//std::cout<<a.getBitHeader()<<std::endl;
	//std::cout<<a.getBitMessageType()<<std::endl;
	//std::cout<<int('\0')<<0<<std::endl;
	//	std::cout<<char(21)<<"\t"<<int(a.getBitMessage()[3])<<std::endl;
	
	if (!a.printParsedMessage(std::string("TestMessage15.txt")))
		return 1;
delete m3;
delete m4;    
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

