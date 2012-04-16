/**
 *
 * \file message_parser.cpp
 * \author Mircea Cretu Stancu
 * \brief rebuilds an ACLMessage from a parsed bit-efficient encoded received message
 * 
 * \version 1.0
 */
#include <set>

#include <boost/variant/get.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/variant.hpp>

#include "message_parser.h"
#include "grammar_bitefficient.h"


namespace fipa { 
namespace acl {


MessageParser::MessageParser()
{
}

MessageParser::~MessageParser()
{
}

bool MessageParser::parseData(const std::string storage, ACLMessage &msg)
{
	typedef fipa::acl::bitefficient_grammar<std::string::const_iterator> bitefficient_grammar;
	bitefficient_grammar grammar;
	fipa::acl::Message parseTree;

	std::string::const_iterator iter = storage.begin();
	std::string::const_iterator end = storage.end();
	bool r = parse(iter, end, grammar, parseTree);
	
	if(r && iter == end)
	{
		return buildMessage(parseTree, msg);
  	}
	
	return false;
}

bool MessageParser::buildMessage(Message parsedMsg, ACLMessage &msg)
{
	msg.setPerformative(parsedMsg.type);
	buildParameters(parsedMsg.parameters, msg);
	
	return true;
}

void MessageParser::buildParameters(std::vector<MessageParameter> parsedParams, ACLMessage &msg)
{
    std::vector<MessageParameter>::iterator it = parsedParams.begin();
    UserdefParam param;
 
    for (; it != parsedParams.end(); ++it)
    {	
        if (!buildPredefMessageParameters(*it,msg)) 
        {
	   
	    param = buildUserdefParameter(*it);
	    msg.addUserdefParam(param);
        }
    }
}

int MessageParser::buildPredefMessageParameters(MessageParameter param,ACLMessage &msg)
{
    if (param.name == "sender") 	 {buildSender(param, msg); return 1;}
    if (param.name == "receiver")	 {buildReceiver(param, msg); return 1;}
    
    if (param.name == "content")	 {buildContent(param, msg);  return 1;}
    
    if (param.name == "reply-with")	 {buildReplyWith(param, msg); return 1;}
    if (param.name == "reply-by")	 {buildReplyBy1(param, msg); return 1;}
    if (param.name == "in-reply-to")	 {buildInReplyTo(param, msg); return 1;}
    if (param.name == "reply-to")	 {buildReplyTo(param, msg); return 1;}
    if (param.name == "language")	 {buildLanguage(param, msg); return 1;}
    if (param.name == "encoding")	 {buildEncoding(param, msg); return 1;}
    if (param.name == "ontology")	 {buildOntology(param, msg); return 1;}
    if (param.name == "protocol")	 {buildProtocol(param, msg); return 1;}
    if (param.name == "conversation-id") {buildConversationID(param, msg); return 1;}
    
    return 0;
}

void MessageParser::buildSender(MessageParameter param,ACLMessage &msg)
{
    fipa::acl::ParameterValue mydata; 
    mydata = param.data;
    
    AgentIdentifier temp = boost::get<AgentIdentifier>(mydata);
    AgentID sender = buildAgentID(temp);
    
    msg.setSender(sender);
}

AgentID MessageParser::buildAgentID(AgentIdentifier agent)
{
    AgentID retAg;
    retAg.setName(agent.name);
    // setting the addresses
    if (!agent.addresses.empty()) 
    {
        std::vector<std::string>::iterator it = agent.addresses.begin();
        for(; it != agent.addresses.end(); ++it)
	  retAg.addAddress(*it);
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

void MessageParser::buildResolvers(AgentID &workAg, AgentIdentifier agent)
{
    struct AgentIdentifier unbuiltres;
    
    std::vector<fipa::acl::Resolver>::iterator it = agent.resolvers.begin();
    for (; it != agent.resolvers.end(); ++it)
    {
        unbuiltres = it->get(); // get function of the boost::recursive_wrapper
        AgentID res = buildAgentID(unbuiltres);
        workAg.addResolver(res);
    }
}

void MessageParser::buildAgentParameters(AgentID &workAg, AgentIdentifier agent)
{
    std::vector<UserDefinedParameter>::iterator it = agent.parameters.begin();
    for (; it != agent.parameters.end(); ++it)
    {
        UserdefParam _parameter = buildUserdefParameter(*it);
        workAg.addUserdefParam(_parameter);
    }
}

UserdefParam MessageParser::buildUserdefParameter(UserDefinedParameter param)
{
    std::string value = boost::get<std::string>(param.data); // boost::get() -> to return a specific type value from a boost::variant

    UserdefParam retParam;
    retParam.setName(param.name);
    retParam.setValue(value);
    
    return retParam;
}

UserdefParam MessageParser::buildUserdefParameter(MessageParameter param)
{
    std::string value = boost::get<std::string>(param.data); // boost::get() -> to return a specific type value from a boost::variant

    UserdefParam retParam;
    retParam.setName(param.name);
    retParam.setValue(value);
    
    return retParam;

}

void MessageParser::buildReceiver(MessageParameter param, ACLMessage &msg)
{
   AgentID receiver;    

   std::vector<AgentIdentifier> unbuiltReceivers = boost::get<std::vector<AgentIdentifier> >(param.data); // boost::get() -> to return a specific type value from a boost::variant
   std::vector<AgentIdentifier>::iterator it = unbuiltReceivers.begin();
   for (; it!= unbuiltReceivers.end(); ++it)
   {
       receiver = buildAgentID(*it);
       msg.addReceiver(receiver);
   }
   
}

void MessageParser::buildReplyWith(MessageParameter param, ACLMessage &msg)
{          
    std::string replyWith  = boost::get<std::string>(param.data);
    msg.setReplyWith(replyWith);
}

void MessageParser::buildReplyBy1(MessageParameter param, ACLMessage &msg)
{
    DateTime dateTime = boost::get<DateTime>(param.data);
    std::string mydate = dateTime.toString();
    /// the first char in the parsed date is for relative time, that feature is not yet supported by the generator, thus the byte is 
    /// simply discarded; if it is to be used in the reconstruction of the message, this is where it should be done
    if (mydate.c_str()[0] == ' ') {mydate.assign(mydate,1,2000);}
         
    //if (msg.setReplyBy1(mydate) != 0) 
    msg.setReplyBy1(mydate);
}

void MessageParser::buildReplyTo(MessageParameter param, ACLMessage &msg)
{
    AgentID agentId;
    std::vector<AgentIdentifier> agentIds = boost::get<std::vector<AgentIdentifier> >(param.data);
     
    std::vector<AgentIdentifier>::iterator it = agentIds.begin();
    for (; it != agentIds.end(); ++it)
    {
        agentId = buildAgentID(*it);
        msg.addReplyTo(agentId);
    }
}

void MessageParser::buildInReplyTo(MessageParameter param, ACLMessage &msg)
{
    std::string inReplyTo = boost::get<std::string>(param.data);
    msg.setInReplyTo(inReplyTo);
}

void MessageParser::buildLanguage(MessageParameter param, ACLMessage &msg)
{
    std::string language = boost::get<std::string>(param.data);
    msg.setLanguage(language);
}

void MessageParser::buildEncoding(MessageParameter param, ACLMessage &msg)
{
    std::string encoding = boost::get<std::string>(param.data);
    msg.setEncoding(encoding);
}

void MessageParser::buildOntology(MessageParameter param, ACLMessage &msg)
{
    std::string ontology = boost::get<std::string>(param.data);
    msg.setOntology(ontology);
}

void MessageParser::buildProtocol(MessageParameter param, ACLMessage &msg)
{
    std::string protocol = boost::get<std::string>(param.data);
    msg.setProtocol(protocol);
}

void MessageParser::buildConversationID(MessageParameter param, ACLMessage &msg)
{
    std::string id = boost::get<std::string>(param.data);
    msg.setConversationID(id);
}

   
void MessageParser::buildContent(MessageParameter param,ACLMessage &msg)
{
    ByteSequence byteSequence = boost::get<ByteSequence>(param.data);
    std::string content;
    byteSequence.toRawDataString(&content);
    msg.setContent(content);
}


}}
