#include "message_parser.h"


#include <set>

#include <boost/variant/get.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/variant.hpp>

namespace fipa { 
namespace acl {


MessageParser::MessageParser()
{
}

MessageParser::~MessageParser()
{
}


ACLMessage*  MessageParser::parseData(const std::string storage)
{
 
	fipa::acl::ACLMessage* message = NULL;
	  
	/*
	std::string storage;
	std::vector<uint_8_t>::iterator it = data.begin();
	for(it; it != data.end(); it++)
	    storage.append(*it);
	*/
        
	typedef fipa::acl::bitefficient_grammar<std::string::const_iterator> bitefficient_grammar;
	bitefficient_grammar grammar;
	fipa::acl::Message parseTree;

	using boost::spirit::ascii::space;
	std::string::const_iterator iter = storage.begin();
	std::string::const_iterator end = storage.end();
	bool r = phrase_parse(iter, end, grammar, space, parseTree);

	if(r && iter == end)
	{
	//ACLMessage *rebuilt;
	message = buildMessage(parseTree);
  	}
	
	std::cout<< message->getReplyWith()<<"\n";
	
	return message;
  // use the stuff as shown in the main.cpp an use the tools given in bitefficient_grammar
  // to populate the ACLMessage

}

ACLMessage* MessageParser::buildMessage(Message parsedMsg)
{
	ACLMessage *msg = new ACLMessage();
	msg->setPerformative(parsedMsg.type);
	
	std::cout<<"type set "<< msg->getPerformative()<<std::endl;
	if (parsedMsg.parameters.empty()) std::cout<<"empty param list\n";
	
	buildParameters(parsedMsg.parameters,msg);
	
	std::cout<< msg->getReplyWith()<<"\n";
	
	return msg;
	
}

void MessageParser::buildParameters(std::vector<MessageParameter> parsedParams,ACLMessage* msg)
{
    std::vector<MessageParameter>::iterator it = parsedParams.begin();
    UserdefParam *_param;
    std::cout<<"build Parameters call\n";
    for (it; it != parsedParams.end(); it++)
    {	
        std::cout<<"param iteration\n";
        if (!buildPredefMessageParameters(*it,msg)) 
        {
	  std::cout<<"not predef\n";
	  _param = buildUserdefParameter(*it);
	  std::cout<<"userdef param returned\n";
	  msg->addUserdefParam(_param);
	  
        }
    }
}

int MessageParser::buildPredefMessageParameters(MessageParameter param,ACLMessage* msg)
{
    
    if (!param.name.compare(std::string("sender"))) 	{buildSender(param, msg); std::cout<<"1\n"; return 1;}
    if (!param.name.compare(std::string("receiver")))	{buildReceiver(param, msg); std::cout<<"2\n"; return 1;}
    
    if (!param.name.compare(std::string("content")))	{std::cout<<"content\n"; buildContent(param, msg);  return 1;}
    
    if (!param.name.compare(std::string("reply-with")))	{buildReplyWith(param, msg); std::cout<<"3\n"; return 1;}
    if (!param.name.compare(std::string("reply-by")))	{buildReplyBy(param, msg); std::cout<<"4\n"; return 1;}
    if (!param.name.compare(std::string("in-reply-to")))	{buildInReplyTo(param, msg); std::cout<<"5\n"; return 1;}
    if (!param.name.compare(std::string("reply-to")))	{buildReplyTo(param, msg); std::cout<<"6\n"; return 1;}
    if (!param.name.compare(std::string("language")))	{buildLanguage(param, msg); std::cout<<"7\n"; return 1;}
    if (!param.name.compare(std::string("encoding")))	{buildEncoding(param, msg); std::cout<<"8\n"; return 1;}
    if (!param.name.compare(std::string("ontology")))	{buildOntology(param, msg); std::cout<<"9\n"; return 1;}
    if (!param.name.compare(std::string("protocol")))	{buildProtocol(param, msg); std::cout<<"10\n"; return 1;}
    if (!param.name.compare(std::string("conversation-id"))){buildConversationID(param, msg); std::cout<<"11\n"; return 1;}
    
    std::cout<<"no-predef\n";
    return 0;
}

void MessageParser::buildSender(MessageParameter param,ACLMessage* msg)
{
    
    fipa::acl::ParameterValue mydata; // = new fipa::acl::ParameterValue;
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
    //AgentAID *_resolver;
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
    UserDefinedParameterValue *_data = new UserDefinedParameterValue();
    *_data = param.data;
    
    std::string *_value = boost::get<std::string>(_data); // boost::get() -> to return a specific type value from a boost::variant
    retParam->setName(param.name);
    retParam->setValue(*_value);
    
    delete _data;
    return retParam;
}

UserdefParam* MessageParser::buildUserdefParameter(MessageParameter param)
{
    std::cout<<"userdef call\n";
    UserdefParam *retParam = new UserdefParam();
    // I create a pointer with the value of the data so that I can call the boost::get() function that I want (*variant<...>*)
    ParameterValue *_data = new ParameterValue();
    *_data = param.data;
    
    std::string *_value = boost::get<std::string>(_data); // boost::get() -> to return a specific type value from a boost::variant
    retParam->setName(param.name);
    retParam->setValue(*_value);
    
    delete _data;
    return retParam;
}

void MessageParser::buildReceiver(MessageParameter param, ACLMessage *msg)
{
    AgentAID* _receiver;
    // I create a pointer with the value of the data so that I can call the boost::get() function that I want (*variant<...>*)
    ParameterValue *_data = new ParameterValue();
    *_data = param.data;
    
    
    std::vector<AgentID>* unbuiltReceivers = boost::get<std::vector<AgentID> >(_data); // boost::get() -> to return a specific type value from a boost::variant
    std::vector<AgentID>::iterator it = unbuiltReceivers->begin();
    for (it; it!= unbuiltReceivers->end(); it++)
    {
        _receiver = buildAgentAID(*it);
        msg->addReceiver(_receiver);
    }
    delete _data;
}

void MessageParser::buildReplyWith(MessageParameter param, ACLMessage *msg)
{          
     std::string _value = std::string();
     _value = boost::get<std::string>(param.data);
     
     msg->setReplyWith(_value);
}

void MessageParser::buildReplyBy(MessageParameter param, ACLMessage *msg)
{
    ParameterValue *mydata = new ParameterValue;
     *mydata = param.data;
          
     std::string *_value = new std::string();
     _value = boost::get<std::string>(mydata);
     msg->setReplyBy1(*_value);
     
     delete mydata;
     delete _value;
}

void MessageParser::buildReplyTo(MessageParameter param, ACLMessage *msg)
{
    ParameterValue *mydata = new ParameterValue;
    AgentAID *result;
     *mydata = param.data;
          
     std::vector<AgentID>* _value;
     _value = boost::get<std::vector<AgentID> >(mydata);
     
     std::vector<AgentID>::iterator it = _value->begin();
     for (it; it != _value->end(); it++)
     {
         result = buildAgentAID(*it);
         msg->addReplyTo(result);
     }
     
     delete mydata;
     delete _value;
     
}

void MessageParser::buildInReplyTo(MessageParameter param, ACLMessage *msg)
{
     ParameterValue *mydata = new ParameterValue;
     *mydata = param.data;
          
     std::string *_value = new std::string();
     _value = boost::get<std::string>(mydata);
     msg->setInReplyTo(*_value);
     
     delete mydata;
     delete _value;
}

void MessageParser::buildLanguage(MessageParameter param, ACLMessage *msg)
{
     ParameterValue *mydata = new ParameterValue;
     *mydata = param.data;
          
     std::string *_value = new std::string();
     _value = boost::get<std::string>(mydata);
     msg->setLanguage(*_value);
     
     delete mydata;
     delete _value;
}

void MessageParser::buildEncoding(MessageParameter param, ACLMessage *msg)
{
     ParameterValue *mydata = new ParameterValue;
     *mydata = param.data;
          
     std::string *_value = new std::string();
     _value = boost::get<std::string>(mydata);
     msg->setEncoding(*_value);
     
     delete mydata;
     delete _value;
}

void MessageParser::buildOntology(MessageParameter param, ACLMessage *msg)
{
     ParameterValue *mydata = new ParameterValue;
     *mydata = param.data;
          
     std::string *_value = new std::string();
     _value = boost::get<std::string>(mydata);
     msg->setOntology(*_value);
     
     delete mydata;
     delete _value;
}

void MessageParser::buildProtocol(MessageParameter param, ACLMessage *msg)
{
     ParameterValue *mydata = new ParameterValue;
     *mydata = param.data;
          
     std::string *_value = new std::string();
     _value = boost::get<std::string>(mydata);
     msg->setProtocol(*_value);
     
     delete mydata;
     delete _value;
}

void MessageParser::buildConversationID(MessageParameter param, ACLMessage *msg)
{
     ParameterValue *mydata = new ParameterValue;
     *mydata = param.data;
          
     std::string *_value = new std::string();
     _value = boost::get<std::string>(mydata);
     msg->setConversationID(*_value);
     
     delete mydata;
     delete _value;
}

   
void MessageParser::buildContent(MessageParameter param,ACLMessage* msg)
{
     ByteSequence temp;
     std::string _content = std::string();

     temp = boost::get<ByteSequence>(param.data);
     _content = temp.toString();
     
     msg->setContent(_content);
     std::cout<<"content set\n";
}


}}
