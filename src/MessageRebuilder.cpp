

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>

#include <boost/variant/get.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/variant.hpp>

#include "MessageRebuilder.h"
//#include "ACLMessage.h"
//#include "AgentAID.h"
//#include "../../message-parser/src/grammar_bitefficient.h"


namespace fipa {

namespace acl {

const std::string MessageRebuilder::PredefMessageParams[] = {"sender","receiver","content","reply-with","reply-by","in-reply-to","reply-to",
				    "language","encoding","ontology","protocol","conversation-id"};
    
ACLMessage* MessageRebuilder::buildMessage(Message parsedMsg)
{
	ACLMessage *msg = new ACLMessage();
	msg->setPerformative(parsedMsg.type);
	
	buildParameters(parsedMsg.parameters,msg);
	
}

void MessageRebuilder::buildParameters(std::vector<MessageParameter> parsedParams,ACLMessage* msg)
{
    std::vector<MessageParameter>::iterator it = parsedParams.begin();
    UserdefParam *_param;
    
    for (it; it != parsedParams.end(); it++)
    {
        if (!buildPredefMessageParameters(*it,msg)) 
        {
	  _param = buildUserdefParameter(*it);
	  msg->addUserdefParam(_param);
	  
        }
    }
}

int MessageRebuilder::buildPredefMessageParameters(MessageParameter param,ACLMessage* msg)
{
    
    if (!param.name.compare(std::string("sender"))) 	{buildSender(param, msg); return 1;}
    if (!param.name.compare(std::string("receiver")))	{buildReceiver(param, msg); return 1;}
    
    if (!param.name.compare(std::string("content")))	{buildContent(param, msg); return 1;}
    
    if (!param.name.compare(std::string("reply-with")))	{buildReplyWith(param, msg); return 1;}
    if (!param.name.compare(std::string("reply-by")))	{buildReplyBy(param, msg); return 1;}
    if (!param.name.compare(std::string("in-reply-to")))	{buildInReplyTo(param, msg); return 1;}
    if (!param.name.compare(std::string("reply-to")))	{buildReplyTo(param, msg); return 1;}
    if (!param.name.compare(std::string("language")))	{buildLanguage(param, msg); return 1;}
    if (!param.name.compare(std::string("encoding")))	{builEncoding(param, msg); return 1;}
    if (!param.name.compare(std::string("ontology")))	{buildOntology(param, msg); return 1;}
    if (!param.name.compare(std::string("protocol")))	{buildProtocol(param, msg); return 1;}
    if (!param.name.compare(std::string("conversation-id"))){buildConversationID(param, msg); return 1;}
    
    return 0;
}

void MessageRebuilder::buildSender(MessageParameter param,ACLMessage* msg)
{
    
    fipa::acl::ParameterValue *mydata = new fipa::acl::ParameterValue;
    *mydata = param.data;
    
    AgentID temp = boost::get<AgentID>(mydata);
    AgentAID *_sender = buildAgentAID(temp);
    
    msg->setSender(_sender);
    
}

AgentAID* MessageRebuilder::buildAgentAID(AgentID agent)
{
    AgentAID* retAg = new AgentAID(agent.name);
    
    // setting the adresses
    if (!agent.addresses.empty()) 
    {
        std::vector<std::string>::iterator it = agent.addresses.begin();
        for(it; it != agent.addresses.end(); it++)
	  retAg->addAdress(*it);
    }
    // building resolvers
    if (!agent.resolvers.empty())
    {
        buildResolvers(retAg,agent);
    }
    //building set of userdefined parameters
    if (!agent.parameters.empty())
    {
        buildAgentParameters(retAg,agent);
    }
    
    
    return retAg;
}

void MessageRebuilder::buildResolvers(AgentAID *workAg, AgentID agent)
{
    //AgentAID *_resolver;
    struct AgentID unbuiltres;
    
    
    std::vector<fipa::acl::Resolver>::iterator it = agent.resolvers.begin();
    for (it; it != agent.resolvers.end(); it++)
    {
        unbuiltres = it->get(); // get function of the boost::recursive_wrapper
        workAg->addResolver(buildAgentAID(unbuiltres));
    }
}

void MessageRebuilder::buildAgentParameters(AgentAID* workAg, AgentID agent)
{
    
    std::vector<UserDefinedParameter>::iterator it = agent.parameters.begin();
    for (it; it != agent.parameters.end(); it++)
    {
        UserdefParam *_parameter = buildUserdefParameter(*it);
        workAg->addUserdefParam(_parameter);
    }
}

UserdefParam* MessageRebuilder::buildUserdefParameter(UserDefinedParameter param)
{
    UserdefParam *retParam = new UserdefParam();
    // I create a pointer with the value of the data so that I can call the boost::get() function that I want (*variant<...>*)
    UserDefinedParameterValue *_data = new UserdDefinedParameterValue();
    *_data = param.data;
    
    std::string _value = boost::get<std::string>(_data); // boost::get() -> to return a specific type value from a boost::variant
    retParam->setName(param.name);
    retParam->setValue(_value);
    
    delete _data;
    return retParam;
}

void MessageRebuilder::buildReceiver(MessageParameter param, ACLMessage *msg)
{
    AgentAID* _receiver;
    // I create a pointer with the value of the data so that I can call the boost::get() function that I want (*variant<...>*)
    ParameterValue *_data = new ParameterValue();
    *_data = param.data;
    
    
    std::vector<AgentID> unbuiltReceivers = boost::get<Receiver>(_data); // boost::get() -> to return a specific type value from a boost::variant
    std::vector<AgentID>::iterator it = unbuiltReceivers.begin();
    for (it; it!= unbuiltReceivers.end(); it++)
    {
        _receiver = buildAgentAID(*it);
        msg->addReceiver(_receiver);
    }
    delete _data;
}

void MessageRebuilder::buildReplyWith(MessageParameter param, ACLMessage *msg)
{
    ParameterValue *mydata = new ParameterValue;
     *mydata = param.data;
          
     std::string *_value = new std::string();
     _value = boost::get<std::string>(mydata);
     msg->setReplyWith(*_value);
     
     delete mydata;
     delete _value;
}

void MessageRebuilder::buildReplyBy(MessageParameter param, ACLMessage *msg)
{
    ParameterValue *mydata = new ParameterValue;
     *mydata = param.data;
          
     std::string *_value = new std::string();
     _value = boost::get<std::string>(mydata);
     msg->setReplyBy1(*_value);
     
     delete mydata;
     delete _value;
}

void MessageRebuilder::buildInReplyTo(MessageParameter param, ACLMessage *msg)
{
    ParameterValue *mydata = new ParameterValue;
    AgentAID *result;
     *mydata = param.data;
          
     std::vector<AgentID> _value;
     _value = boost::get<std::vector<AgentID> >(mydata);
     
     std::vector<AgentID>::iterator it = _value.begin();
     for (it; it != _value.end(); it++)
     {
         result = buildAgentAID(*it);
         msg->addReplyTo(result);
     }
     
     delete mydata;
     
}

void MessageRebuilder::buildReplyTo(MessageParameter param, ACLMessage *msg)
{
     ParameterValue *mydata = new ParameterValue;
     *mydata = param.data;
          
     std::string *_value = new std::string();
     _value = boost::get<std::string>(mydata);
     msg->setReplyTo(*_value);
     
     delete mydata;
     delete _value;
}

void MessageRebuilder::builLanguage(MessageParameter param, ACLMessage *msg)
{
     ParameterValue *mydata = new ParameterValue;
     *mydata = param.data;
          
     std::string *_value = new std::string();
     _value = boost::get<std::string>(mydata);
     msg->setLanguage(*_value);
     
     delete mydata;
     delete _value;
}

void MessageRebuilder::buildEncoding(MessageParameter param, ACLMesssage *msg)
{
     ParameterValue *mydata = new ParameterValue;
     *mydata = param.data;
          
     std::string *_value = new std::string();
     _value = boost::get<std::string>(mydata);
     msg->setEncoding(*_value);
     
     delete mydata;
     delete _value;
}

void MessageRebuilder::builOntology(MessageParameter param, ACLMessage *msg)
{
     ParameterValue *mydata = new ParameterValue;
     *mydata = param.data;
          
     std::string *_value = new std::string();
     _value = boost::get<std::string>(mydata);
     msg->setOntology(*_value);
     
     delete mydata;
     delete _value;
}

void MessageParameter::buildProtocol(MessageParameter param, ACLMessage *msg)
{
     ParameterValue *mydata = new ParameterValue;
     *mydata = param.data;
          
     std::string *_value = new std::string();
     _value = boost::get<std::string>(mydata);
     msg->setProtocol(*_value);
     
     delete mydata;
     delete _value;
}

void MessageRebuilder::builConversationID(MessageParameter param, ACLMessage *msg)
{
     ParameterValue *mydata = new ParameterValue;
     *mydata = param.data;
          
     std::string *_value = new std::string();
     _value = boost::get<std::string>(mydata);
     msg->setConversationID(*_value);
     
     delete mydata;
     delete _value;
}

   
void MessageRebuilder::buildContent(MessageParameter param,ACLMessage* msg)
{
     ParameterValue *mydata = new ParameterValue;
     *mydata = param.data;
     //boost::variant<std::string, fipa::acl::AgentID, std::vector<fipa::acl::AgentID>, fipa::acl::ByteSequence, fipa::acl::DateTime, fipa::acl::ByteString >
     //mydata = param.data;
     
     
     std::string *_content =  std::string();
     _content = boost::get<std::string>(mydata);
     msg->setContent(*_content);
     
     delete mydata;
     delete _content;
}




} // end of namespace
} // end of namespace
