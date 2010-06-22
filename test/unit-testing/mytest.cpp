#include <iostream>

#include "../../src/ACLMessage.h"
#include "../../src/ACLMessageOutputParser.h"
#include "../../src/AgentAID.h"

using namespace std;
using namespace fipa::acl;

int main()
{
    AgentAID *a1 = new AgentAID(); //no leak
    AgentAID *a2 = new AgentAID(); 
    AgentAID *a3 = new AgentAID();
    AgentAID *a4 = new AgentAID();
    AgentAID *a5 = new AgentAID();
    
    //UserdefParam *p1 = new UserdefParam();
    
    //ACLMessage *m1 = new ACLMessage();
    
    
    
    //a1->addResolver(a2); // 24 bytes leak
    //a1->addResolver(a3); // 48 bytes leak
    //a1->addResolver(a4); // 72 bytes leak
    //m1->setSender(a1);
    //m1->addReplyTo(a2);
    //m1->addReplyTo(a3);
    //m1->addReceiver(a1);
    //m1->addUserdefParam(p1);
    a3->addResolver(a4);
    a2->addResolver(a3);
    cout<<sizeof(*a4)<<endl;
    
    
    a1->addResolver(a2); // 72 bytes lost
    a1->addResolver(a2);
    *a5 = *a1;
    
    
    cout<< ((*a1 == *a5)?"equal\n":"not equal\n");
    //a5->addResolver(a4);
    cout<< ((*a1 == *a5)?"equal\n":"not equal\n");
      
    //cout<< sizeof(a2->getUserdefParams())<<'\t'<<sizeof(a2)<<endl;
    
    delete a1;
    delete a2;
    delete a3;
    delete a4;
    delete a5;
    //delete m1;
    //delete p1;
    
    return 0;
}