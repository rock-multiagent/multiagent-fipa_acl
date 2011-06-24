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
    if (!(*msg).getReplyBy1().empty()) cout<<"reply by1:\t"<< msg->getReplyBy1()<<endl;
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
    for(it; it != myset->end(); it++)
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
        for(it; it != agent->getAddresses()->end(); it++)
	  cout<<"\t\t\t"<<*it<<endl;
    }
    if (!agent->getResolvers()->empty()) {cout<<"\t\tresolvers:\t\n"; printAgentIDset(agent->getResolvers());}
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
/*
                std::set<fipa::acl::AgentID*>* receivers = aclmessage->getAllReceivers();
                std::set<fipa::acl::AgentID*>::iterator it;

                std::vector<std::string> recvs;
                for (it = receivers->begin(); it != receivers->end(); it++) 
                        recvs.push_back((*it)->getName());

/*
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
	
	
	AgentID *a1 = new AgentID(std::string("r1"));
	(*a1).addAddress(std::string("adr1"));
	(*a1).addAddress(std::string("adr2"));	
	(*m3).setSender(a1);
	(*a1).addUserdefParam(p1);
	
	AgentID *a2 = new AgentID(std::string("r2"));
	AgentID *a3 = new AgentID(std::string("r3"));
	AgentID *a4 = new AgentID(std::string("r4"));
	AgentID *a5 = new AgentID(std::string("r5"));
	AgentID *a6 = new AgentID(std::string("r6"));
	AgentID *a7 = new AgentID(std::string("r7"));
	UserdefParam *p4 = new UserdefParam();
	
	
	(*a6).addAddress(std::string("adr1"));
	(*a6).addAddress(std::string("adr2"));	
	(*m3).addReplyTo(a6);
	//(*a6).addResolver(a7);
	//(*a6).addResolver(a2);
	
	(*m3).addReceiver(a3);
	(*m3).addReceiver(a1);
	
	(*a7).addAddress(std::string("adr1"));
	(*a7).addAddress(std::string("adr2"));	
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
	
	
	
	
/*
int found_one = 0;

AgentID *a100 = new AgentID;
a100->setName(string("a100"));
a100->addAddress(string("address1"));
a100->addAddress(string("address2"));
if (a100->getAddresses()->empty()) cout<<"empty\n";
AgentID *a10 = new AgentID;
a10->setName(string("a10"));
a10->addAddress(string("address1"));

a100->addResolver(a10);
a100->addResolver(a1);
a100->addResolver(a7);
a100->addResolver(a6);

AgentID *a101 =  new AgentID();
*a101 = *a100;
printAgentID(a101);
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



//a11->getAddresses()->erase(a11->getAddresses()->begin());
//cout<< (*a11->getAddresses()->begin())<<endl;


    /*

    std::set<std::string>* addrA = a101.getAddresses();
    std::set<std::string>* addrB = a11.getAddresses();
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

if (a100->getAddresses()->empty()) cout<<"empty\n";
if (a10->getAddresses()->empty()) cout<<"empty\n";

AgentID *comp1 = new AgentID(std::string("agname"));
AgentID *comp2 = new AgentID(std::string("agname"));
cout<<"control\n";
comp1->addResolver(a100);
cout <<"control\n";
comp2->addResolver(a100);


    std::set<AgentID*>* agentsA = (*comp1).getResolvers();
    cout<<"retrieved resolvers\n";
    std::set<AgentID*>* agentsB = (*comp2).getResolvers();
    cout<<"retrieved resolvers\n";
    std::set<AgentID*>::iterator ait = agentsA->begin();
    std::set<AgentID*>::iterator bit = agentsB->begin();
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


 


  /*
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
    */    
    return 0;
}


