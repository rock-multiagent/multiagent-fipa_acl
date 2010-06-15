/**
 *
 * \file message_parser.cpp
 * \author Mircea Cretu Stancu
 * \brief rebuilds an ACLMessage from a parsed bit-efficient encoded received message
 * 
 * \version 1.0
 */
#include "grammar_bitefficient.h"
#include "message_parser.h"


#include <set>

#include <boost/variant/get.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/variant.hpp>

// ENCODING need to be set according to grammar 
//namespace boost::spirit::ascii;
namespace encoding = boost::spirit::standard;

namespace fipa { 
namespace acl {


MessageParser::MessageParser()
{
}

MessageParser::~MessageParser()
{
}


bool MessageParser::parseData(const std::string storage, ACLMessage* msg)
{
	/*
	std::string storage;
	std::vector<uint_8_t>::iterator it = data.begin();
	for(it; it != data.end(); it++)
	    storage.append(*it);
	*/
        
	typedef fipa::acl::bitefficient_grammar<std::string::const_iterator> bitefficient_grammar;
	bitefficient_grammar grammar;
	fipa::acl::Message parseTree;

	std::string::const_iterator iter = storage.begin();
	std::string::const_iterator end = storage.end();
	bool r = phrase_parse(iter, end, grammar, encoding::space, parseTree);
	
	if(r && iter == end)
	{
		return buildMessage(parseTree, msg);
  	}
	
	return false;


}

bool MessageParser::buildMessage(Message parsedMsg, ACLMessage* msg)
{
	msg->setPerformative(parsedMsg.type);
	buildParameters(parsedMsg.parameters, msg);
	
	return true;
}

void MessageParser::buildParameters(std::vector<MessageParameter> parsedParams, ACLMessage* msg)
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

int MessageParser::buildPredefMessageParameters(MessageParameter param,ACLMessage* msg)
{
    
    if (!param.name.compare(std::string("sender"))) 	{buildSender(param, msg); return 1;}
    if (!param.name.compare(std::string("receiver")))	{buildReceiver(param, msg); return 1;}
    
    if (!param.name.compare(std::string("content")))	{buildContent(param, msg);  return 1;}
    
    if (!param.name.compare(std::string("reply-with")))	{buildReplyWith(param, msg); return 1;}
    if (!param.name.compare(std::string("reply-by")))	{buildReplyBy1(param, msg); return 1;}
    if (!param.name.compare(std::string("in-reply-to")))	{buildInReplyTo(param, msg); return 1;}
    if (!param.name.compare(std::string("reply-to")))	{buildReplyTo(param, msg); return 1;}
    if (!param.name.compare(std::string("language")))	{buildLanguage(param, msg); return 1;}
    if (!param.name.compare(std::string("encoding")))	{buildEncoding(param, msg); return 1;}
    if (!param.name.compare(std::string("ontology")))	{buildOntology(param, msg); return 1;}
    if (!param.name.compare(std::string("protocol")))	{buildProtocol(param, msg); return 1;}
    if (!param.name.compare(std::string("conversation-id"))){buildConversationID(param, msg); return 1;}
    
    return 0;
}

void MessageParser::buildSender(MessageParameter param,ACLMessage* msg)
{
    
    fipa::acl::ParameterValue mydata; 
    mydata = param.data;
    
    AgentID temp = boost::get<AgentID>(mydata);
    AgentAID *_sender = buildAgentAID(temp);
    
    msg->setSender(_sender);
    
}

AgentAID* MessageParser::buildAgentAID(AgentID agent)
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

void MessageParser::buildResolvers(AgentAID *workAg, AgentID agent)
{
    struct AgentID unbuiltres;
    
    std::vector<fipa::acl::Resolver>::iterator it = agent.resolvers.begin();
    for (it; it != agent.resolvers.end(); it++)
    {
        unbuiltres = it->get(); // get function of the boost::recursive_wrapper
        workAg->addResolver(buildAgentAID(unbuiltres));
    }
}

void MessageParser::buildAgentParameters(AgentAID* workAg, AgentID agent)
{
    
    std::vector<UserDefinedParameter>::iterator it = agent.parameters.begin();
    for (it; it != agent.parameters.end(); it++)
    {
        UserdefParam *_parameter = buildUserdefParameter(*it);
        workAg->addUserdefParam(_parameter);
    }
}

UserdefParam* MessageParser::buildUserdefParameter(UserDefinedParameter param)
{
    UserdefParam *retParam = new UserdefParam();
    // I create a pointer with the value of the data so that I can call the boost::get() function that I want (*variant<...>*)

    
    std::string _value = boost::get<std::string>(param.data); // boost::get() -> to return a specific type value from a boost::variant
    retParam->setName(param.name);
    retParam->setValue(_value);
    
    return retParam;
}

UserdefParam* MessageParser::buildUserdefParameter(MessageParameter param)
{
    //std::cout<<"userdef call\n";
    UserdefParam *retParam = new UserdefParam();
    
    std::string _value = boost::get<std::string>(param.data); // boost::get() -> to return a specific type value from a boost::variant
    retParam->setName(param.name);
    retParam->setValue(_value);
    
    return retParam;

}

void MessageParser::buildReceiver(MessageParameter param, ACLMessage *msg)
{
    AgentAID* _receiver;    
    
    std::vector<AgentID> unbuiltReceivers = boost::get<std::vector<AgentID> >(param.data); // boost::get() -> to return a specific type value from a boost::variant
    std::vector<AgentID>::iterator it = unbuiltReceivers.begin();
    for (it; it!= unbuiltReceivers.end(); it++)
    {
        _receiver = buildAgentAID(*it);
        msg->addReceiver(_receiver);
    }
   
}

void MessageParser::buildReplyWith(MessageParameter param, ACLMessage *msg)
{          
     std::string _value = std::string();
     _value = boost::get<std::string>(param.data);
     
     msg->setReplyWith(_value);
}

void MessageParser::buildReplyBy1(MessageParameter param, ACLMessage *msg)
{
     DateTime _value;
     _value = boost::get<DateTime>(param.data);
     
     msg->setReplyBy1(_value.toString());
}

void MessageParser::buildReplyTo(MessageParameter param, ACLMessage *msg)
{
    AgentAID *result;
          
     std::vector<AgentID> _value;
     _value = boost::get<std::vector<AgentID> >(param.data);
     
     std::vector<AgentID>::iterator it = _value.begin();
     for (it; it != _value.end(); it++)
     {
         result = buildAgentAID(*it);
         msg->addReplyTo(result);
     }
}

void MessageParser::buildInReplyTo(MessageParameter param, ACLMessage *msg)
{
     std::string _value = std::string();
     _value = boost::get<std::string>(param.data);
     msg->setInReplyTo(_value);
}

void MessageParser::buildLanguage(MessageParameter param, ACLMessage *msg)
{
     std::string _value = std::string();
     _value = boost::get<std::string>(param.data);
     msg->setLanguage(_value);
}

void MessageParser::buildEncoding(MessageParameter param, ACLMessage *msg)
{
     std::string _value = std::string();
     _value = boost::get<std::string>(param.data);
     msg->setEncoding(_value);
}

void MessageParser::buildOntology(MessageParameter param, ACLMessage *msg)
{
     std::string _value = std::string();
     _value = boost::get<std::string>(param.data);
     msg->setOntology(_value);
}

void MessageParser::buildProtocol(MessageParameter param, ACLMessage *msg)
{
     std::string _value = std::string();
     _value = boost::get<std::string>(param.data);
     msg->setProtocol(_value);
}

void MessageParser::buildConversationID(MessageParameter param, ACLMessage *msg)
{
     std::string _value = std::string();
     _value = boost::get<std::string>(param.data);
     msg->setConversationID(_value);
}

   
void MessageParser::buildContent(MessageParameter param,ACLMessage* msg)
{
     ByteSequence temp;
     std::string _content = std::string();

     temp = boost::get<ByteSequence>(param.data);
     _content = temp.toString();
     
     msg->setContent(_content);
}


}}
