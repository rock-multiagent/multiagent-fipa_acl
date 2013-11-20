/**
 * \file bitefficient_message_parser.cpp
 * \author Mircea Cretu Stancu
 * \brief rebuilds an ACLMessage from a parsed bit-efficient encoded received message
 */
#include <set>

#include <boost/variant/get.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/variant.hpp>
#include <base/logging.h>

#include "bitefficient_message_parser.h"
#include "grammar/grammar_bitefficient.h"


namespace fipa { 
namespace acl {

bool BitefficientMessageParser::parseData(const std::string& storage, ACLMessage &msg)
{
    typedef fipa::acl::bitefficient::Message<std::string::const_iterator> bitefficient_message_grammar;
    bitefficient_message_grammar grammar;
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

bool BitefficientMessageParser::buildMessage(const Message& parsedMsg, ACLMessage &msg)
{
	msg.setPerformative(parsedMsg.type);
	buildParameters(parsedMsg.parameters, msg);
	
	return true;
}

void BitefficientMessageParser::buildParameters(const std::vector<message::Parameter>& parsedParams, ACLMessage &msg)
{
    std::vector<message::Parameter>::const_iterator it = parsedParams.begin();
 
    for (; it != parsedParams.end(); ++it)
    {	
        try {
            buildPredefMessageParameters(*it,msg);
        } catch(const std::runtime_error& e)
        {
	    UserdefParam param = buildUserdefParameter(*it);
            LOG_INFO("Creating userdefined parameter name: '%s' value: '%s'", param.getName().c_str(), param.getValue().c_str());
	    msg.addUserdefParam(param);
        }
    }
}

void BitefficientMessageParser::buildPredefMessageParameters(const message::Parameter& param, ACLMessage &msg)
{
    // Parameter that require custom conversion from internal data types
    if (param.name == "sender") 
    {
        buildSender(param, msg);
        return;
    } 
    if (param.name == "receiver") 
    {
        buildReceiver(param, msg);
        return;
    }
    if (param.name == "content") 
    {
        buildContent(param, msg);
        return;
    } 
    if (param.name == "reply-by") 
    {
        buildReplyBy(param, msg);
        return;
    } 
    if (param.name == "reply-to") 
    {
        buildReplyTo(param, msg);
        return;
    } 
   
    // Parameters that use string data type
    std::string value;
    try {
        value = boost::get<std::string>(param.data); 
    } catch (const boost::bad_get& e)
    {
        LOG_ERROR("Failed getting data for parameter '%s'", param.name.c_str());
        throw e;
    }

    if (param.name == "in-reply-to") 
    {
        msg.setInReplyTo(value);
        return;
    } 
    if (param.name == "reply-with") 
    {
        msg.setReplyWith(value);
        return;
    } 
    if (param.name == "language")
    {
        msg.setLanguage(value);
        return;
    } 
    if (param.name == "encoding") 
    {
        msg.setEncoding(value);
        return;
    } 
    if (param.name == "ontology") 
    {
        msg.setOntology(value);
        return;
    } 
    if (param.name == "protocol") 
    {
        msg.setProtocol(value);
        return;
    } 
    
    if (param.name == "conversation-id") 
    {
        msg.setConversationID(value);
        return;
    }

    std::string errorMsg = "Message parameter '" + param.name + "' is not predefined";
    LOG_ERROR("%s",errorMsg.c_str());
    throw std::runtime_error(errorMsg);
}

void BitefficientMessageParser::buildSender(const message::Parameter& param, ACLMessage &msg)
{
    const AgentIdentifier& temp = boost::get<AgentIdentifier>(param.data);
    AgentID sender = buildAgentID(temp);
    
    msg.setSender(sender);
}

AgentID BitefficientMessageParser::buildAgentID(const AgentIdentifier& agent)
{
    AgentID retAg;
    retAg.setName(agent.name);
    // setting the addresses
    if (!agent.addresses.empty()) 
    {
        std::vector<std::string>::const_iterator it = agent.addresses.begin();
        for(; it != agent.addresses.end(); ++it)
        {
	  retAg.addAddress(*it);
        }
    }
    // building resolvers
    if (!agent.resolvers.empty())
    {
        buildResolvers(agent, retAg);
    }
    //building set of userdefined parameters
    if (!agent.parameters.empty())
    {
        buildAgentParameters(agent, retAg);
    }
    
    
    return retAg;
}

void BitefficientMessageParser::buildResolvers(const AgentIdentifier& agentIdentifier, AgentID& outAgentID)
{
    std::vector<fipa::acl::Resolver>::const_iterator it = agentIdentifier.resolvers.begin();
    for (; it != agentIdentifier.resolvers.end(); ++it)
    {
        const AgentIdentifier& resolver = it->get(); // get function of the boost::recursive_wrapper
        AgentID agent = buildAgentID(resolver);
        outAgentID.addResolver(agent);
    }
}

void BitefficientMessageParser::buildAgentParameters(const AgentIdentifier& agentIdentifier, AgentID& outAgentID)
{
    std::vector<UserDefinedParameter>::const_iterator it = agentIdentifier.parameters.begin();
    for (; it != agentIdentifier.parameters.end(); ++it)
    {
        UserdefParam parameter = buildUserdefParameter(*it);
        outAgentID.addUserdefParam(parameter);
    }
}

UserdefParam BitefficientMessageParser::buildUserdefParameter(const UserDefinedParameter& param)
{
    // boost::get() -> to return a specific type value from a boost::variant
    const std::string& value = boost::get<std::string>(param.data); 

    UserdefParam retParam;
    retParam.setName(param.name);
    retParam.setValue(value);
    
    return retParam;
}

UserdefParam BitefficientMessageParser::buildUserdefParameter(const message::Parameter& param)
{
    const std::string& value = boost::get<std::string>(param.data); // boost::get() -> to return a specific type value from a boost::variant

    UserdefParam retParam;
    retParam.setName(param.name);
    retParam.setValue(value);
    
    return retParam;

}

void BitefficientMessageParser::buildReceiver(const message::Parameter& param, ACLMessage &msg)
{
   // boost::get() -> to return a specific type value from a boost::variant
   const std::vector<AgentIdentifier>& unbuiltReceivers = boost::get<std::vector<AgentIdentifier> >(param.data); 
   std::vector<AgentIdentifier>::const_iterator it = unbuiltReceivers.begin();
   for (; it!= unbuiltReceivers.end(); ++it)
   {
       AgentID receiver = buildAgentID(*it);
       msg.addReceiver(receiver);
   }
}

void BitefficientMessageParser::buildReplyBy(const message::Parameter& param, ACLMessage &msg)
{
    const DateTime& dateTime = boost::get<DateTime>(param.data);
    base::Time date = dateTime.toTime();
    msg.setReplyBy(date);
}

void BitefficientMessageParser::buildReplyTo(const message::Parameter& param, ACLMessage &msg)
{
    const std::vector<AgentIdentifier>& agentIds = boost::get<std::vector<AgentIdentifier> >(param.data);
     
    std::vector<AgentIdentifier>::const_iterator it = agentIds.begin();
    for (; it != agentIds.end(); ++it)
    {
        AgentID agentId = buildAgentID(*it);
        msg.addReplyTo(agentId);
    }
}

void BitefficientMessageParser::buildContent(const message::Parameter& param,ACLMessage &msg)
{
    const ByteSequence& byteSequence = boost::get<ByteSequence>(param.data);
    std::string content;
    byteSequence.toRawDataString(&content);
    msg.setContent(content);
}

}}
