#include "rice/Class.hpp"
#include "rice/Constructor.hpp"
#include "rice/String.hpp"
#include "rice/Array.hpp"

#include <stdint.h>
#include <vector>
#include <iostream>
#include <fipa_acl/message_generator/message_generator.h>
#include <fipa_acl/message_parser/message_parser.h>

using namespace Rice;

typedef std::vector< std::string > StringVector;
typedef std::vector< fipa::acl::AgentID > AgentIDVector;
typedef std::vector< fipa::acl::UserdefParam > ParameterVector;
typedef std::vector< uint8_t > ByteVector;

Data_Type<StringVector> stringVector;
Data_Type<AgentIDVector> aidVector;
Data_Type<ParameterVector> parameterVector;
Data_Type<ByteVector> byteVector;

Data_Type<fipa::acl::ACLMessage> rb_cFipaMessage;
Data_Type<fipa::acl::AgentID> rb_cAgentID;
Data_Type<fipa::acl::UserdefParam> rb_cUserDefinedParameters;

static Module rb_mFIPA;

template<>
StringVector from_ruby< StringVector >(Object obj)
{
	Data_Object<StringVector> strings(obj, stringVector);
	return *strings;
}

template<>
Object to_ruby<StringVector>(const StringVector& v)
{
	Array strings;
	StringVector::const_iterator it;
	for(it = v.begin(); it != v.end(); it++)
	{
		strings.push(String(*it));
	}

	return strings;
}


template<>
AgentIDVector from_ruby< AgentIDVector >(Object obj)
{
	Data_Object<AgentIDVector> ids(obj, aidVector);
	return *ids;
}

template<>
Object to_ruby< AgentIDVector >(const AgentIDVector& v)
{
	Array ids;
	AgentIDVector::const_iterator it;
	for( it = v.begin(); it != v.end(); it++)
	{
		ids.push(Data_Object<fipa::acl::AgentID>(new fipa::acl::AgentID(*it) ));
	}

	return ids;
}

template<>
ParameterVector from_ruby< ParameterVector >(Object obj)
{
	Data_Object<ParameterVector> params(obj, parameterVector);
	return *params;
}

template<>
Object to_ruby< ParameterVector >(const ParameterVector& v)
{
	Array params;
	ParameterVector::const_iterator it;
	for( it = v.begin(); it != v.end(); it++)
	{
		params.push(Data_Object<fipa::acl::UserdefParam>(new fipa::acl::UserdefParam(*it) ));
	}

	return params;
}

template<>
Object to_ruby< ByteVector >(const std::vector<uint8_t>& obj)
{
	Array bytes;
        
	int size = obj.size();
	for( int i = 0; i < size; i++)
	{
		bytes.push(obj[i]);
	}

	return bytes;
}

template<>
ByteVector from_ruby< ByteVector >(Object obj)
{
	Data_Object< ByteVector> bytes(obj, byteVector);
	return *bytes;	
}

// Convert std::vector<string> to ruby Array
Array wrap_getAddresses(Object aid)
{
   // the self
   Data_Object<fipa::acl::AgentID> id(aid, rb_cAgentID);

   //Array list of agents
   Array agents;
   std::vector<std::string> addresses = id->getAddresses();
   agents = to_ruby<StringVector>(addresses);
   
   return agents;
}

// Convert std::vector<string> to ruby Array
Array wrap_getResolvers(Object aid)
{
   // the self
   Data_Object<fipa::acl::AgentID> id(aid, rb_cAgentID);
   Array resolvers;
   AgentIDVector aids = id->getResolvers();
   resolvers = to_ruby<AgentIDVector>(aids);
	
   return resolvers;
}

Array wrap_getUserDefinedParameters(Object aid)
{
   // the self
   Data_Object<fipa::acl::AgentID> id(aid, rb_cAgentID);
   Array paramArray;
   ParameterVector params = id->getUserdefParams();
   paramArray = to_ruby<ParameterVector>(params);
	
   return paramArray;
}


Array wrap_toByteVector(Object message)
{

   Data_Object<fipa::acl::ACLMessage> msg(message, rb_cFipaMessage);

   std::string bitefficientMsg = fipa::acl::MessageGenerator::create(*msg, fipa::acl::message_format::BITEFFICIENT);
   int size = bitefficientMsg.size();

   std::vector<uint8_t> bytes;
   for(int i = 0; i < size; i++)
   {
	bytes.push_back(bitefficientMsg[i]);
   } 

   return to_ruby<ByteVector>(bytes); 
}


/**
* Conversion of a string, that contains an array of bytes as core data into a FipaMessage
* The array of bytes will be parsed in the message object
* throw an exception if the parsing step fails
*/
Object wrap_fromByteString(Object self, String byteString)
{
	Data_Object<fipa::acl::ACLMessage> msg(self, rb_cFipaMessage);	

	std::string data; 
	int size = byteString.length();

	for(int i=0; i < size; i++)
	{
		data += (char) byteString[i];
	}

	fipa::acl::MessageParser parser;
	if(!parser.parseData(data, *msg))
	{
		throw Exception(rb_eRuntimeError, "FIPA::ACLMessage: data could not be parsed");
	}

	return self;
}

Object wrap_setPerformative(Object self, Symbol performative)
{
	Data_Object<fipa::acl::ACLMessage> msg(self, rb_cFipaMessage);

	// Replace underscore with dash to convert symbols to performative label
	std::string performativeString = performative.str();
	std::string::iterator it;
	while(true)
	{
	    size_t pos = performativeString.find_first_of("_");
	    if(pos == std::string::npos)
		break;

	    performativeString.replace(pos,1,"-");
	}

	msg->setPerformative(performativeString); 

	return self;
}

Symbol wrap_getPerformative(Object self)
{
	Data_Object<fipa::acl::ACLMessage> msg(self, rb_cFipaMessage);
	std::string performative = msg->getPerformative();

	std::string::iterator it;
	while(true)
	{
	    size_t pos = performative.find_first_of("-");
	    if(pos == std::string::npos)
		break;

	    performative.replace(pos,1,"_");
	}

	return Symbol(performative);
}



/*
template<typename T>
class Vector
{
	typedef std::vector<T> TVector; 
	TVector tVectorInstance;

	Array toArray(Object o)
	{
		Data_Object<T> typedVector(o, tVectorInstance);
		Array array;
		//array = to_ruby<T>( *typedVector );
		return array;
	}
	
	template<>
	Array to_ruby<TVector>(const TVector& v)
	{
		Array array;
		
		std::vector<T>::const_iterator it;
		for( it = v.begin(); it != v.end(); it++)
		{
			array.push(Data_Object<T>(new T(*it) ));
		}
	
		return array;
	}

	TVector from_ruby(Object obj)
	{
		Data_Object<TVector> tVector(obj, tVectorInstance);
		return *tVector;
	}
	

};
*/

/**
* Initialise method in order to 
*/
extern "C"
void Init_fipamessage_ruby()
{

 // Define module FIPA
 rb_mFIPA = define_module("FIPA");

 rb_cUserDefinedParameters = define_class_under<fipa::acl::UserdefParam>(rb_mFIPA, "UserDefinedParameter")
   .define_constructor(Constructor<fipa::acl::UserdefParam, const std::string&>(), Arg("name"))
   .define_constructor(Constructor<fipa::acl::UserdefParam>())
   .define_method("getName", &fipa::acl::UserdefParam::getName)
   .define_method("setName", &fipa::acl::UserdefParam::setName, (Arg("name")) )
   .define_method("getValue", &fipa::acl::UserdefParam::getValue, (Arg("name") ) )
   .define_method("setValue", &fipa::acl::UserdefParam::setValue, Arg("value") );

 rb_cAgentID = define_class_under<fipa::acl::AgentID>(rb_mFIPA,"AgentId")
   .define_constructor(Constructor<fipa::acl::AgentID, const std::string&>() )
   .define_method("getName", &fipa::acl::AgentID::getName)
   .define_method("addAddress", &fipa::acl::AgentID::addAddress, (Arg("address")))
   .define_method("getAddresses", &wrap_getAddresses)
   .define_method("addResolver", &fipa::acl::AgentID::addResolver, (Arg("agentid")))
   .define_method("getResolvers", &wrap_getResolvers)
   .define_method("deleteResolver", &fipa::acl::AgentID::deleteResolver, (Arg("agentid") ))
   .define_method("addUserDefinedParameter", &fipa::acl::AgentID::addUserdefParam, (Arg("param")))
   .define_method("getUserDefinedParameters", &wrap_getUserDefinedParameters);

 rb_cFipaMessage = define_class_under<fipa::acl::ACLMessage>(rb_mFIPA, "ACLMessage")
   .define_constructor(Constructor<fipa::acl::ACLMessage>())
   .define_method("setPerformative", &wrap_setPerformative)
   .define_method("getPerformative", &wrap_getPerformative)
   .define_method("addReceiver", &fipa::acl::ACLMessage::addReceiver, Arg("receiver"))
   .define_method("clearReceivers", &fipa::acl::ACLMessage::clearReceivers)
   .define_method("getReceivers", &fipa::acl::ACLMessage::getAllReceivers)
   .define_method("setProtocol", &fipa::acl::ACLMessage::setProtocol, Arg("protocol_name"))
   .define_method("getProtocol", &fipa::acl::ACLMessage::getProtocol)
   .define_method("setOntology", &fipa::acl::ACLMessage::setOntology, Arg("ontology_name"))
   .define_method("getOntology", &fipa::acl::ACLMessage::getOntology)
   .define_method("setEncoding", &fipa::acl::ACLMessage::setEncoding, Arg("encoding_name"))
   .define_method("getEncoding", &fipa::acl::ACLMessage::getEncoding)
   .define_method("setLanguage", &fipa::acl::ACLMessage::setLanguage, Arg("language_name"))
   .define_method("getLanguage", &fipa::acl::ACLMessage::getLanguage)
   .define_method("setContent", &fipa::acl::ACLMessage::setContent, Arg("content_string"))
   .define_method("getContent", &fipa::acl::ACLMessage::getContent)
   .define_method("setSender", &fipa::acl::ACLMessage::setSender, Arg("sender_name"))
   .define_method("getSender", &fipa::acl::ACLMessage::getSender)
   .define_method("addReplyTo", &fipa::acl::ACLMessage::addReplyTo, Arg("agent_id"))
   .define_method("setInReplyTo", &fipa::acl::ACLMessage::setInReplyTo, Arg("reply_to"))
   .define_method("getInReplyTo", &fipa::acl::ACLMessage::getInReplyTo)
   .define_method("setReplyWith", &fipa::acl::ACLMessage::setReplyWith, Arg("reply_with"))
   .define_method("getReplyWith", &fipa::acl::ACLMessage::getReplyWith)
   .define_method("setConversationID", &fipa::acl::ACLMessage::setConversationID, Arg("conversation_id"))
   .define_method("getConversationID", &fipa::acl::ACLMessage::getConversationID)
   .define_method("to_byte_array", &wrap_toByteVector)
   .define_method("from_byte_string", &wrap_fromByteString)
   //.define_method("setReplyBy", 
   //.define_method("addUserDefinedParameter", &fipa::acl::ACLMessage::addUserdefParam)
   //.define_method("getUserDefinedParameters", &wrap_getUserDefinedParameters)
   ;
}
