
#include <iostream>
#include <set>
#include <iterator>
#include <string>
//#include <conio.h>
#include "ACLMessage.h"
#include "ACLMessageOutputParser.h"
#include "UserdefParam.h"

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
    ACLMessage* m = new ACLMessage();
	(*m).setPerformative(ACLMessage::perfs[ACLMessage::REQUEST_WHENEVER]);
    (*m).setConversationID("plm");
    (*m).setReplyBy1("01010");

	UserdefParam *p = new UserdefParam();
//std::cout<<"1\n";
	(*p).setName(std::string("cretzu")); 
	(*p).setValue(std::string("mare"));
	(*m).addUserdefParam(p); 
UserdefParam *p1 = new UserdefParam();

	(*p1).setName(std::string("stancu")); 
	(*p1).setValue(std::string("mic")); ;
	(*m).addUserdefParam(p1); 

/*
	AgentAID *a1 = new AgentAID(std::string("aaaaa"));
	AgentAID *a2 = new AgentAID(std::string("euaassddfffadaaa"));
	(*m).addReceiver(a1); (*m).addReceiver(a2);
*/  
  //printm(*m);
    //getch();
	ACLMessageOutputParser a = ACLMessageOutputParser();
	a.setMessage(m);
	std::cout<<a.getBitMessage()<<std::endl<<a.getBitMessage().length()<<std::endl;
	//std::cout<<a.getBitHeader()<<std::endl;
	//std::cout<<a.getBitMessageType()<<std::endl;
	//std::cout<<int('\0')<<0<<std::endl;
	//	std::cout<<char(21)<<"\t"<<int(a.getBitMessage()[3])<<std::endl;
	
	a.printParsedMessage(std::string("TestMessage.txt"));
    
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

