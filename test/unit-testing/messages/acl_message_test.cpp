#include <boost/test/auto_unit_test.hpp> 
 
#include <iostream>
#include <fstream>
#include <fipa_acl/bitefficient_message.h>

using namespace std;
using namespace fipa::acl;

BOOST_AUTO_TEST_SUITE(ACLMessageTest);

ACLMessage m1, m2, m3;
UserdefParam p1,p2,p3,p4;
AgentID a1,a2,a3,a4,a5;

void printAgentIDset( vector<AgentID> &myset);
void printAgentID( AgentID &agent);
void printUserdefParamset( vector<UserdefParam> &params);

void setupTest()
{
    string addr1 = string("addr1");
    string addr2 = string("addr2");
    string addr3 = string("addr3");
	
    p1.setName(string("param"));
    p1.setValue(string("!@#$%^&*("));
    p2 = p1;
    p3.setName(string("param3"));
    p3.setValue(string("value3"));
    p4.setName(string("param4"));
    p4.setValue(string("value4"));
    
    m1.setPerformative(ACLMessage::REQUEST);
    m1.setLanguage(string("test language"));
    m1.setContent(string("test content"));
    m1.setEncoding(string("test encoding"));
    m1.setOntology(string("test ontology"));
    m1.setReplyWith(string("test reply_with"));
    base::Time time0 = base::Time::fromString("20110917-00:00:30", base::Time::Seconds);
    m1.setReplyBy(time0);
    m1.setInReplyTo(string("test in_reply_to"));
    m1.setConversationID(string("test conversationID"));
    m1.setProtocol(string("test-protocol"));
    
    m2.setPerformative(ACLMessage::REQUEST);
    m2.setLanguage(string("test language"));
    m2.setContent(string("test content"));
    m2.setEncoding(string("test encoding"));
    m2.setOntology(string("test ontology"));
    m2.setReplyWith(string("test reply_with"));
    base::Time time1 = base::Time::fromString("20110917-00:00:30", base::Time::Seconds);
    m2.setReplyBy(time1);
    m2.setInReplyTo(string("test in_reply_to"));
    m2.setConversationID(string("test conversationID"));
    m2.setProtocol(string("test-protocol"));
    
    a1.setName(string("agent1"));
    a1.addAddress(addr1);
    a1.addAddress(addr2);
    a1.addAddress(addr3);
    a1.addUserdefParam(p1);
    a1.addUserdefParam(p3);
    
    
    BOOST_REQUIRE(!a3.setName(string("agent3 5")));
    BOOST_REQUIRE(a4.setName(string("agent4")));
    BOOST_REQUIRE(!a5.setName(string("agent3 5")));
    
    a3.addAddress(addr1);
    a4.addAddress(addr1);
    a5.addAddress(addr1);
    
    a3.addUserdefParam(p1);
    
    a4.addUserdefParam(p4);
    a5.addUserdefParam(p2); // p1 = p2(should)
    
    a2 = a1;
    a5.addResolver(a1);
    a3.addResolver(a2); // a1 = a2(should)
    
    m1.setSender(a4);
    m2.setSender(a4);
    
    m1.addReceiver(a3);
    m2.addReceiver(a3);
    m1.addReceiver(a4);
    m2.addReceiver(a4);
    m1.addUserdefParam(p1);
    m2.addUserdefParam(p1);
}

void MessageEqTest()
{
    BOOST_CHECK_EQUAL((m1 == m2), true);
    m1.addReceiver(a3);
    BOOST_CHECK_EQUAL((m1 == m2), true);
    m1.addReplyTo(a2);
    m2.addReplyTo(a1);
    BOOST_CHECK_EQUAL((m1 == m2), true);
    m1.addReceiver(a1);
    BOOST_CHECK_EQUAL((m1 == m2), false);
    m3 = m1;
    BOOST_CHECK_EQUAL((m3 == m2), false);
    m2.addReceiver(a2);
    BOOST_CHECK_EQUAL((m1 == m2), true);
    
    
}

void AgentEqTest()
{
    
    BOOST_CHECK_EQUAL((a1 == a2),true);
    BOOST_CHECK_EQUAL((a5==a3),true);
    a3.addUserdefParam(p3);
    BOOST_CHECK_EQUAL((a5==a3),false);
    a5.addUserdefParam(p3);
    BOOST_CHECK_EQUAL((a5==a3),true);
    //AgentID::setResCompDepth(5);
    BOOST_CHECK_EQUAL(AgentID::compareEqual(a5,a3,5),true);
    //AgentID::setResCompDepth(1);
    a5.deleteResolver(a1); 
    a2.addResolver(a4);
    a5.addResolver(a2);
    BOOST_CHECK_EQUAL((a5 == a3),true);
    a5.addResolver(a2);
    BOOST_CHECK_EQUAL((a5 == a3),true);
    //AgentID::setResCompDepth(5);
    BOOST_CHECK_EQUAL(AgentID::compareEqual(a5,a3,5),false);
    a5.addResolver(a1);
    BOOST_CHECK_EQUAL((a5 == a3),false);
    std::vector<AgentID> vec = a5.getResolvers();
    std::vector<AgentID> ::iterator it = vec.begin();
    while(it!=vec.end()) {a5.deleteResolver((*it)); it++;}
    a3 = a5;
    BOOST_CHECK_EQUAL((a5 == a3),true);
    
    
}

void printMessage( ACLMessage &msg)
{
    cout<<"=================================Printing Message=================================\n";
    cout<<"performative:\t"<< msg.getPerformative()<<endl;
    
    if (!msg.getContent().empty()) 
        cout<<"content:\t"<< msg.getContent()<<endl;
    if (!msg.getReplyWith().empty()) 
        cout<<"reply with:\t"<< msg.getReplyWith()<<endl;
    if (!msg.getReplyBy().isNull()) 
        cout<<"reply by:\t"<< msg.getReplyBy().toString()<<endl;
    if (!msg.getInReplyTo().empty()) 
        cout<<"in reply to:\t"<< msg.getInReplyTo()<<endl;
    if (!msg.getLanguage().empty()) 
        cout<<"language:\t"<< msg.getLanguage()<<endl;
    if (!msg.getEncoding().empty()) 
        cout<<"encoding:\t"<< msg.getEncoding()<<endl;
    if (!msg.getOntology().empty()) 
        cout<<"ontology:\t"<< msg.getOntology()<<endl;
    if (!msg.getProtocol().empty()) 
        cout<<"protocol:\t"<< msg.getProtocol()<<endl;
    if (!msg.getConversationID().empty()) 
        cout<<"conversation id:\t"<< msg.getConversationID()<<endl;
    cout<<"sender:\n"; 
    AgentID aid = msg.getSender();
    printAgentID(aid);
    if (!(msg.getAllReceivers().empty())) 
				  { cout<<"receivers:\n"; 
				    std::vector<AgentID> vec = msg.getAllReceivers();
				    printAgentIDset(vec);}
    if (!msg.getAllReplyTo().empty()) 
			        { cout<<"reply to:\n"; 
				std::vector<AgentID> vec = msg.getAllReplyTo();
				printAgentIDset(vec);}
    if (!msg.getUserdefParams().empty()) 
				 {  vector<UserdefParam> params = msg.getUserdefParams();
				    printUserdefParamset(params); }

}

void printUserdefParamset( vector<UserdefParam> &params)
{
    cout<<"\t\t==================User Defined Parameters==================\n";
    vector<UserdefParam>::iterator it = params.begin();
    for(; it != params.end(); ++it)
    {
        cout<<"\t\tparam name:\t"<< (*it).getName()<<endl;
        cout<<"\t\tparam value:\t"<< (*it).getValue()<<"\n\n";
    }
}

void printAgentID( AgentID &agent)
{
    cout<<"\t==================Agent AID==================\n";
    if (!agent.getName().empty()) cout<<"\t\tname:\t"<< agent.getName()<<endl;
    if (!(agent.getAddresses().empty()))
    {
        cout<<"\t\taddresses:\t\n";
        vector<string>::const_iterator it = agent.getAddresses().begin();
        for(; it != (agent.getAddresses().end()); ++it)
	  cout<<"\t\t\t"<<*it<<endl;
    }
    if (!(agent.getResolvers().empty())) {cout<<"\t\tresolvers:\t\n"; 
				vector<AgentID> vec = agent.getResolvers();
				printAgentIDset(vec);}
    if (!(agent.getUserdefParams().empty())) {cout<<"\t\tUser Defined Parameters:\t\n";
				    vector<UserdefParam> params = agent.getUserdefParams();
				    printUserdefParamset(params);}
}

void printAgentIDset( vector<AgentID> &myset)
{
    cout<<"\t==================Agent AID set==================\n";
    vector<AgentID>::iterator it = myset.begin();
    for(; it != myset.end(); it++)
        printAgentID(*it);
}


BOOST_AUTO_TEST_CASE(message_test)
{
    setupTest();
    MessageEqTest();
    AgentEqTest();
}

BOOST_AUTO_TEST_SUITE_END()

