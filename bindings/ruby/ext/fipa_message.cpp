#include "rice/Class.hpp"
#include "rice/Constructor.hpp"
#include "rice/String.hpp"
#include "rice/Array.hpp"
#include "rice/Enum.hpp"

#include <stdint.h>
#include <vector>
#include <iostream>
#include <fipa_acl/fipa_acl.h>
#include <fipa_acl/message_generator/envelope_generator.h>
#include <fipa_acl/message_parser/envelope_parser.h>

using namespace Rice;
using namespace fipa::acl;

typedef std::vector< std::string > StringVector;
typedef std::vector< AgentID > AgentIDVector;
typedef std::vector< UserdefParam > ParameterVector;
typedef std::vector< uint8_t > ByteVector;

Data_Type<StringVector> stringVector;
Data_Type<AgentIDVector> aidVector;
Data_Type<ParameterVector> parameterVector;
Data_Type<ByteVector> byteVector;

Data_Type<ACLEnvelope> rb_cFipaEnvelope;
Data_Type<ACLBaseEnvelope> rb_cFipaBaseEnvelope;
Data_Type<ACLMessage> rb_cFipaMessage;
Data_Type<AgentID> rb_cAgentID;
Data_Type<UserdefParam> rb_cUserDefinedParameters;

Enum<representation::Type> rb_eRepresentationType;

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
		ids.push(Data_Object<AgentID>(new AgentID(*it) ));
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
		params.push(Data_Object<UserdefParam>(new UserdefParam(*it) ));
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
   Data_Object<AgentID> id(aid, rb_cAgentID);

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
   Data_Object<AgentID> id(aid, rb_cAgentID);
   Array resolvers;
   AgentIDVector aids = id->getResolvers();
   resolvers = to_ruby<AgentIDVector>(aids);
	
   return resolvers;
}

Array wrap_getUserDefinedParameters(Object aid)
{
   // the self
   Data_Object<AgentID> id(aid, rb_cAgentID);
   Array paramArray;
   ParameterVector params = id->getUserdefParams();
   paramArray = to_ruby<ParameterVector>(params);
	
   return paramArray;
}


Array wrap_toByteVector(Object message)
{

   Data_Object<ACLMessage> msg(message, rb_cFipaMessage);

   std::string bitefficientMsg = MessageGenerator::create(*msg, representation::BITEFFICIENT);
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
	Data_Object<ACLMessage> msg(self, rb_cFipaMessage);	

	std::string data; 
	int size = byteString.length();

	for(int i=0; i < size; i++)
	{
		data += (char) byteString[i];
	}

	MessageParser parser;
	if(!parser.parseData(data, *msg))
	{
		throw std::runtime_error("FIPA::ACLMessage: data could not be parsed");
	}

	return msg;
}

Object wrap_setPerformative(Object self, Symbol performative)
{
	Data_Object<ACLMessage> msg(self, rb_cFipaMessage);

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
	Data_Object<ACLMessage> msg(self, rb_cFipaMessage);
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

/**
 * ACLEnvelope methods
 */
Array wrap_envelope_getTo(Object self)
{
    Data_Object<ACLEnvelope> envelope(self, rb_cFipaEnvelope);
    ACLBaseEnvelope baseEnvelope = envelope->flattened();
    AgentIDList agents = baseEnvelope.getTo();

    return to_ruby<AgentIDList>(agents);
}

Object wrap_envelope_getFrom(Object self)
{
    Data_Object<ACLEnvelope> envelope(self, rb_cFipaEnvelope);
    ACLBaseEnvelope baseEnvelope = envelope->flattened();
    AgentID agent = baseEnvelope.getFrom();
    
    Data_Object<AgentID> rubyAgent(new AgentID(agent), rb_cAgentID);
    return rubyAgent;
}

Array wrap_envelope_toByteVector(Object self)
{

   Data_Object<ACLEnvelope> envelope(self, rb_cFipaEnvelope);

   std::string bitefficientEnvelope = EnvelopeGenerator::create(*envelope, representation::BITEFFICIENT);

   int size = bitefficientEnvelope.size();

   std::vector<uint8_t> bytes;
   for(int i = 0; i < size; i++)
   {
       bytes.push_back(bitefficientEnvelope[i]);
   } 

   return to_ruby<ByteVector>(bytes); 
}


/**
* Conversion of a string, that contains an array of bytes as core data into a FIPA::ACLEnvelope
* The array of bytes will be parsed in the envelope object
* throw an exception if the parsing step fails
*/
Object wrap_envelope_fromByteString(Object self, String byteString)
{
	Data_Object<ACLEnvelope> envelope(self, rb_cFipaEnvelope);	

	std::string data; 
	int size = byteString.length();

	for(int i=0; i < size; i++)
	{
		data += (char) byteString[i];
	}

	EnvelopeParser parser;
	if(!parser.parseData(data, *envelope, representation::BITEFFICIENT))
	{
		throw std::runtime_error("FIPA::ACLEnvelope: data could not be parsed");
	}

	return envelope;
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

 rb_cUserDefinedParameters = define_class_under<UserdefParam>(rb_mFIPA, "UserDefinedParameter")
   .define_constructor(Constructor<UserdefParam, const std::string&>(), Arg("name"))
   .define_constructor(Constructor<UserdefParam>())
   .define_method("getName", &UserdefParam::getName)
   .define_method("setName", &UserdefParam::setName, (Arg("name")) )
   .define_method("getValue", &UserdefParam::getValue, (Arg("name") ) )
   .define_method("setValue", &UserdefParam::setValue, Arg("value") );

 rb_cAgentID = define_class_under<AgentID>(rb_mFIPA,"AgentId")
   .define_constructor(Constructor<AgentID, const std::string&>() )
   .define_method("getName", &AgentID::getName)
   .define_method("addAddress", &AgentID::addAddress, (Arg("address")))
   .define_method("getAddresses", &wrap_getAddresses)
   .define_method("addResolver", &AgentID::addResolver, (Arg("agentid")))
   .define_method("getResolvers", &wrap_getResolvers)
   .define_method("deleteResolver", &AgentID::deleteResolver, (Arg("agentid") ))
   .define_method("addUserDefinedParameter", &AgentID::addUserdefParam, (Arg("param")))
   .define_method("getUserDefinedParameters", &wrap_getUserDefinedParameters);

 rb_eRepresentationType = define_enum<representation::Type>("FIPARepresentation")
     .define_value("UNKNOWN",representation::BITEFFICIENT)
     .define_value("BITEFFICIENT",representation::BITEFFICIENT)
     .define_value("STRING",representation::STRING_REP)
     .define_value("XML", representation::XML)
     ;

 rb_cFipaBaseEnvelope = define_class_under<ACLBaseEnvelope>(rb_mFIPA, "ACLBaseEnvelope")
   .define_constructor(Constructor<ACLBaseEnvelope>())
   .define_method("getTo", &ACLBaseEnvelope::getTo)
   .define_method("getFrom", &ACLBaseEnvelope::getFrom)
   .define_method("getComments", &ACLBaseEnvelope::getComments)
   .define_method("getACLRepresentation", &ACLBaseEnvelope::getACLRepresentation)
   .define_method("getPayloadLength", &ACLBaseEnvelope::getPayloadLength)
   .define_method("getDate",&ACLBaseEnvelope::getDate)
   .define_method("getIntendedReceivers", &ACLBaseEnvelope::getIntendedReceivers)
   .define_method("getTransportBehaviour", &ACLBaseEnvelope::getTransportBehaviour)
   ;


 rb_cFipaEnvelope = define_class_under<ACLEnvelope>(rb_mFIPA, "ACLEnvelope")
   .define_constructor(Constructor<ACLEnvelope>())
   // RICE has a limitation of one constructor only, thus we use the insert method
   //.define_constructor(Constructor<ACLEnvelope, const ACLMessage&, const representation::Type&>())
   .define_method("insert", &ACLEnvelope::insert, (Arg("message"), Arg("representation")))
   .define_method("getDeliveryPath", &ACLEnvelope::getDeliveryPath)
   .define_method("getBaseEnvelope", &ACLEnvelope::getBaseEnvelope)
   .define_method("getExtraEnvelopes", &ACLEnvelope::getExtraEnvelopes)
   .define_method("getACLMessage", &ACLEnvelope::getACLMessage)
   .define_method("getTo", &wrap_envelope_getTo)
   .define_method("getFrom", &wrap_envelope_getFrom)
   .define_method("flattened", &ACLEnvelope::flattened)
   .define_method("to_byte_array", &wrap_envelope_toByteVector)
   .define_method("from_byte_string", &wrap_envelope_fromByteString)
   ;

 rb_cFipaMessage = define_class_under<ACLMessage>(rb_mFIPA, "ACLMessage")
   .define_constructor(Constructor<ACLMessage>())
   .define_method("setPerformative", &wrap_setPerformative)
   .define_method("getPerformative", &wrap_getPerformative)
   .define_method("addReceiver", &ACLMessage::addReceiver, Arg("receiver"))
   .define_method("clearReceivers", &ACLMessage::clearReceivers)
   .define_method("getReceivers", &ACLMessage::getAllReceivers)
   .define_method("setProtocol", &ACLMessage::setProtocol, Arg("protocol_name"))
   .define_method("getProtocol", &ACLMessage::getProtocol)
   .define_method("setOntology", &ACLMessage::setOntology, Arg("ontology_name"))
   .define_method("getOntology", &ACLMessage::getOntology)
   .define_method("setEncoding", &ACLMessage::setEncoding, Arg("encoding_name"))
   .define_method("getEncoding", &ACLMessage::getEncoding)
   .define_method("setLanguage", &ACLMessage::setLanguage, Arg("language_name"))
   .define_method("getLanguage", &ACLMessage::getLanguage)
   .define_method("setContent", &ACLMessage::setContent, Arg("content_string"))
   .define_method("getContent", &ACLMessage::getContent)
   .define_method("setSender", &ACLMessage::setSender, Arg("sender_name"))
   .define_method("getSender", &ACLMessage::getSender)
   .define_method("addReplyTo", &ACLMessage::addReplyTo, Arg("agent_id"))
   .define_method("setInReplyTo", &ACLMessage::setInReplyTo, Arg("reply_to"))
   .define_method("getInReplyTo", &ACLMessage::getInReplyTo)
   .define_method("setReplyWith", &ACLMessage::setReplyWith, Arg("reply_with"))
   .define_method("getReplyWith", &ACLMessage::getReplyWith)
   .define_method("setConversationID", &ACLMessage::setConversationID, Arg("conversation_id"))
   .define_method("getConversationID", &ACLMessage::getConversationID)
   .define_method("to_byte_array", &wrap_toByteVector)
   .define_method("from_byte_string", &wrap_fromByteString)
   .define_method("to_s", &ACLMessage::toString)
   //.define_method("setReplyBy", 
   //.define_method("addUserDefinedParameter", &ACLMessage::addUserdefParam)
   //.define_method("getUserDefinedParameters", &wrap_getUserDefinedParameters)
   ;
}
