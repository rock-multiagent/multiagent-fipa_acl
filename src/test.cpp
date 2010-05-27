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
#include <message-generator/ACLMessage.h>
#include <message-generator/ACLMessageOutputParser.h>
#include <message-generator/UserdefParam.h>


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

ACLMessage* m3 = new ACLMessage(ACLMessage::perfs[ACLMessage::REQUEST_WHENEVER]);

	
 
    (*m3).setReplyBy1("20091201010101111");

	  UserdefParam *p = new UserdefParam();
	  
	  (*p).setName(std::string("cretzu")); 
	  (*p).setValue(std::string("mare"));
	  (*m3).addUserdefParam(p); 
	
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
	//(*a6).addResolver(a7);
	//(*a6).addResolver(a2);
	
	(*m3).addReceiver(a3);
	(*m3).addReceiver(a1);
	
	(*a7).addAdress(std::string("adr1"));
	(*a7).addAdress(std::string("adr2"));	
	(*m3).addReplyTo(a7);
	//(*a7).addResolver(a6);

	
	//(*a1).addResolver(a2);	
	(*m3).addReceiver(a2); 
	
	//(*a1).addResolver(a4);	
	(*m3).addReceiver(a4); 
	
	
	//(*a2).addResolver(a5);	

	
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


  

        //std::cout<<"written\n";
  
  	//std::string storage = a.getBitMessage();
	//std::cout<<"storage_set\n";
	//MessageParser rebuilder = MessageParser();
	//std::cout<<"rebuilder created\n";
	//ACLMessage* rebuilt = rebuilder.parseData(storage);
	

	//if (!rebuilt); //std::cout <<"null message returned\n";
	//printMessage(rebuilt);
	
	//if ((*m3) == (*rebuilt))
	//    std::cout<<"messages are equal\n";
	//else std::cout<<"messages are not equal\n";
	
	
	
	

int found_one = 0;

AgentAID *a100 = new AgentAID;
a100->setName(string("a100"));
a100->addAdress(string("adress1"));
a100->addAdress(string("adress2"));
if (a100->getAdresses()->empty()) cout<<"empty\n";
AgentAID *a10 = new AgentAID;
a10->setName(string("a10"));
a10->addAdress(string("adress1"));

a100->addResolver(a10);
a100->addResolver(a1);
a100->addResolver(a7);
a100->addResolver(a6);

AgentAID *a101 =  new AgentAID();
*a101 = *a100;
printAgentAID(a101);
cout<<"\n\n\n\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n\n\n";


ACLMessage *m4 = new ACLMessage();
*m4 = *m3;
printMessage(m3);
cout<<"\n\n\n\n";
//delete a1;
//delete m4->getSender();
//delete m4;
printMessage(m4);



if (*m3 == *m4) cout<<"messages are equal\n";
else cout<<"messages are diferent\n"; 
printMessage(m4);



//a11->getAdresses()->erase(a11->getAdresses()->begin());
//cout<< (*a11->getAdresses()->begin())<<endl;


    /*

    std::set<std::string>* addrA = a101.getAdresses();
    std::set<std::string>* addrB = a11.getAdresses();
    std::set<std::string>::iterator sita = addrA->begin();
    std::set<std::string>::iterator sitb = addrB->begin();
   
    while (sita != addrA->end())
    {
        cout <<"outer\n";
        found_one = 0;
        sitb = addrB->begin();
        while (sitb != addrB->end())
        {
	  cout <<"inner\n";
	  if (!(*sita).compare(*sitb)) 
	  {
	      cout<<"found one\n";
	      addrA->erase(sita);
	      sita = addrA->begin();
	      addrB->erase(sitb);
	      sitb = addrB->end();
	      found_one = 1;
	      
	  } else sitb++;
	  
        }
        if (!found_one) sita++;
	  
    }
    if (!addrA->empty())
        cout<<"11\n";
    if (!addrB->empty())
        cout<<"22\n";

if (a100->getAdresses()->empty()) cout<<"empty\n";
if (a10->getAdresses()->empty()) cout<<"empty\n";
/*
AgentAID *comp1 = new AgentAID(std::string("agname"));
AgentAID *comp2 = new AgentAID(std::string("agname"));
cout<<"control\n";
comp1->addResolver(a100);
cout <<"control\n";
comp2->addResolver(a100);


    std::set<AgentAID*>* agentsA = (*comp1).getResolvers();
    cout<<"retrieved resolvers\n";
    std::set<AgentAID*>* agentsB = (*comp2).getResolvers();
    cout<<"retrieved resolvers\n";
    std::set<AgentAID*>::iterator ait = agentsA->begin();
    std::set<AgentAID*>::iterator bit = agentsB->begin();
    cout<<"iterators created\n";
    
    while (ait != agentsA->end())
    {
        cout <<"outer\n";
        found_one = 0;
        bit = agentsB->begin();
        while (bit != agentsB->end())
        {
	  
	  cout<<"inner\n";
	  if ( (*(*ait)) == (*(*bit))) 
	  {
	      cout<<"found one\n";
	      agentsA->erase(ait);
	      ait = agentsA->begin();
	      agentsB->erase(bit);
	      bit = agentsB->end();
	      found_one = 1;
	      
	  } else bit++;
	  
        }
        if (!found_one) ait++;
	  
    }
    if (!agentsA->empty())
        cout<<"1\n";
    if (!agentsB->empty())
        cout<<"2\n";
    else cout<<"agents are equal\n";

*/
//if ((*comp1) == (*comp2)) cout<<"agents are equal\n";


 


  
    delete m3;
    delete p1;
    //delete a1;
    delete a2;
    delete a3;
    delete a4;
    delete a5;
    delete a6;
    delete a7;
    //delete p3;  
    delete p;
    //delete m4;
    //delete comp1;
    //delete comp2;
    //delete a100;
   

    //delete p2;
        
    return 0;
}


