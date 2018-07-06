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

typedef std::vector< std::string > StringVector;
typedef std::vector< fipa::acl::AgentID > AgentIDVector;
typedef std::vector< fipa::acl::UserdefParam > ParameterVector;
typedef std::vector< uint8_t > ByteVector;

Data_Type<StringVector> stringVector;
Data_Type<AgentIDVector> aidVector;
Data_Type<ParameterVector> parameterVector;
Data_Type<ByteVector> byteVector;

Data_Type<fipa::acl::ReceivedObject> rb_cReceivedObject;
Data_Type<fipa::acl::ACLEnvelope> rb_cFipaEnvelope;
Data_Type<fipa::acl::ACLBaseEnvelope> rb_cFipaBaseEnvelope;
Data_Type<fipa::acl::ACLMessage> rb_cFipaMessage;
Data_Type<fipa::acl::AgentID> rb_cAgentID;
Data_Type<fipa::acl::UserdefParam> rb_cUserDefinedParameters;
Data_Type<fipa::acl::ConversationMonitor> rb_cConversationMonitor;
Data_Type<fipa::acl::Conversation> rb_cConversation;

Enum<fipa::acl::representation::Type> rb_eRepresentationType;

static Module rb_mFIPA;

template<>
Object to_ruby< base::Time >(const base::Time& date)
{
    // the self
    double seconds = date.toSeconds();
    rb_require("time");
    return rb_funcall(rb_cTime, rb_intern("at"), 1, DBL2NUM(seconds) );
}

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
fipa::acl::AgentID from_ruby<fipa::acl::AgentID>(Object obj)
{
    Data_Object<fipa::acl::AgentID> agentId(obj, rb_cAgentID);
    return *agentId;
}

template<>
AgentIDVector from_ruby< AgentIDVector >(Object obj)
{
    Array r_agentList(obj);
    AgentIDVector agentList;
    Array::iterator aIt = r_agentList.begin();
    for(; aIt != r_agentList.end(); ++aIt)
    {
	//Data_Object<AgentIDVector> ids(obj, aidVector);
        agentList.push_back( from_ruby<fipa::acl::AgentID>(*aIt) );
    }
    return agentList;
}

template<>
Object to_ruby< AgentIDVector >(const AgentIDVector& v)
{
	using namespace fipa::acl;
	Array ids;
	AgentIDVector::const_iterator it;
	for( it = v.begin(); it != v.end(); it++)
	{
		ids.push(Data_Object<AgentID>(new AgentID(*it) ));
	}

	return ids;
}

template<>
fipa::acl::ACLBaseEnvelopeList from_ruby < fipa::acl::ACLBaseEnvelopeList >(Object obj)
{
    using namespace fipa::acl;
    Array r_baseEnvelopeList(obj);
    ACLBaseEnvelopeList envelopeList;
    Array::iterator eIt = r_baseEnvelopeList.begin();
    for(; eIt != r_baseEnvelopeList.end(); ++eIt)
    {
        envelopeList.push_back( from_ruby<fipa::acl::ACLBaseEnvelope>(*eIt) );
    }
    return envelopeList;
}

template<>
Object to_ruby< fipa::acl::ACLBaseEnvelopeList >(const fipa::acl::ACLBaseEnvelopeList& e)
{
    using namespace fipa::acl;
    Array envelopes;
    ACLBaseEnvelopeList::const_iterator it;
    for( it = e.begin(); it != e.end(); ++it)
    {
        envelopes.push(Data_Object<ACLBaseEnvelope>(new ACLBaseEnvelope(*it)) );
    }
    return envelopes;
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

template<>
Object to_ruby< fipa::acl::ConversationPtr >(const fipa::acl::ConversationPtr& p)
{
    Data_Object<fipa::acl::Conversation> conversation(new fipa::acl::Conversation(*p.get()), rb_cConversation);
    return conversation;
}

// Convert std::vector<string> to ruby Array
Array wrap_getAddresses(Object aid)
{
    using namespace fipa::acl;
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
    using namespace fipa::acl;
    // the self
    Data_Object<AgentID> id(aid, rb_cAgentID);
    Array resolvers;
    AgentIDVector aids = id->getResolvers();
    resolvers = to_ruby<AgentIDVector>(aids);

    return resolvers;
}

Array wrap_getUserDefinedParameters(Object aid)
{
    using namespace fipa::acl;
    // the self
    Data_Object<AgentID> id(aid, rb_cAgentID);
    Array paramArray;
    ParameterVector params = id->getUserdefParams();
    paramArray = to_ruby<ParameterVector>(params);

    return paramArray;
}


Array wrap_ACLMessage_toByteVector(Object message)
{
    using namespace fipa::acl;
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
Object wrap_ACLMessage_fromByteString(Object self, String byteString)
{
	using namespace fipa::acl;
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

Object wrap_ACLMessage_setPerformative(Object self, Symbol performative)
{
	using namespace fipa::acl;
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

Symbol wrap_ACLMessage_getPerformative(Object self)
{
	using namespace fipa::acl;
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

void wrap_ACLMessage_setReplyBy(Object self, Object rubytime)
{
    Data_Object<fipa::acl::ACLMessage> msg(self, rb_cFipaMessage);

    double floattime = NUM2DBL( rb_funcall(rubytime, rb_intern("to_f"), 0) );
    msg->setReplyBy( base::Time::fromSeconds(floattime) );
}
/**
 * ACLEnvelope methods
 */
Array wrap_ACLEnvelope_getTo(Object self)
{
    using namespace fipa::acl;
    Data_Object<ACLEnvelope> envelope(self, rb_cFipaEnvelope);
    ACLBaseEnvelope baseEnvelope = envelope->flattened();
    AgentIDList agents = baseEnvelope.getTo();

    return to_ruby<AgentIDList>(agents);
}

Array wrap_ACLEnvelope_getExtraEnvelopes(Object self)
{
    using namespace fipa::acl;
    Data_Object<ACLEnvelope> envelope(self, rb_cFipaEnvelope);
    ACLBaseEnvelopeList baseEnvelopes = envelope->getExtraEnvelopes();
    return to_ruby<ACLBaseEnvelopeList>(baseEnvelopes);
}

Object wrap_ACLEnvelope_getFrom(Object self)
{
    using namespace fipa::acl;
    Data_Object<ACLEnvelope> envelope(self, rb_cFipaEnvelope);
    ACLBaseEnvelope baseEnvelope = envelope->flattened();
    AgentID agent = baseEnvelope.getFrom();

    Data_Object<AgentID> rubyAgent(new AgentID(agent), rb_cAgentID);
    return rubyAgent;
}

Array wrap_ACLEnvelope_toByteVector(Object self)
{
    using namespace fipa::acl;
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
Object wrap_ACLEnvelope_fromByteString(Object self, String byteString)
{
	using namespace fipa::acl;
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


Object wrap_ConversationMonitor_getConversationId(Object self)
{
    using namespace fipa::acl;
    Data_Object<Conversation> conversation(self, rb_cConversation);
    fipa::acl::ConversationID id = conversation->getConversationId();
    return String(id);
}

void wrap_ACLBaseEnvelope_setDate(Object self, Object rubytime)
{
    Data_Object<fipa::acl::ACLBaseEnvelope> baseEnvelope(self, rb_cFipaBaseEnvelope);

    double floattime = NUM2DBL( rb_funcall(rubytime, rb_intern("to_f"), 0) );
    baseEnvelope->setDate( base::Time::fromSeconds(floattime) );
}

Object wrap_ACLBaseEnvelope_setTo(Object self, Array array)
{
    using namespace fipa::acl;
    Data_Object<ACLBaseEnvelope> envelope(self, rb_cFipaBaseEnvelope);
    AgentIDList agentIdList = from_ruby<AgentIDList>(array);
    envelope->setTo(agentIdList);
    return envelope;
}

/**
* Initialise method in order to
*/
extern "C"
void Init_fipamessage_ruby()
{
    using namespace fipa::acl;

    // Define module FIPA
    rb_mFIPA = define_module("FIPA");

    rb_cUserDefinedParameters = define_class_under<UserdefParam>(rb_mFIPA, "UserDefinedParameter")
        .define_constructor(Constructor<UserdefParam, const std::string&>(), Arg("name"))
        .define_constructor(Constructor<UserdefParam>())
        .define_method("getName", &UserdefParam::getName)
        .define_method("setName", &UserdefParam::setName, (Arg("name")) )
        .define_method("getValue", &UserdefParam::getValue, (Arg("name") ) )
        .define_method("setValue", &UserdefParam::setValue, Arg("value") )
        .define_method("==", &UserdefParam::operator==, (Arg("param")))
        .define_method("to_s", &UserdefParam::toString);

    rb_cAgentID = define_class_under<AgentID>(rb_mFIPA,"AgentId")
        .define_constructor(Constructor<AgentID, const std::string&>() )
        .define_method("getName", &AgentID::getName)
        .define_method("addAddress", &AgentID::addAddress, (Arg("address")))
        .define_method("getAddresses", &wrap_getAddresses)
        .define_method("addResolver", &AgentID::addResolver, (Arg("agentid")))
        .define_method("getResolvers", &wrap_getResolvers)
        .define_method("deleteResolver", &AgentID::deleteResolver, (Arg("agentid") ))
        .define_method("addUserDefinedParameter", &AgentID::addUserdefParam, (Arg("param")))
        .define_method("getUserDefinedParameters", &wrap_getUserDefinedParameters)
        .define_method("==", &AgentID::operator==, (Arg("agentid")))
        .define_method("to_s",&AgentID::toString);

    rb_eRepresentationType = define_enum<representation::Type>("FIPARepresentation")
        .define_value("UNKNOWN",representation::BITEFFICIENT)
        .define_value("BITEFFICIENT",representation::BITEFFICIENT)
        .define_value("STRING",representation::STRING_REP)
        .define_value("XML", representation::XML)
        ;

    rb_cReceivedObject = define_class_under<ReceivedObject>(rb_mFIPA,
            "ReceivedObject")
        .define_constructor(Constructor<ReceivedObject>())
        .define_method("getBy",&ReceivedObject::getBy)
        .define_method("getFrom",&ReceivedObject::getFrom)
        .define_method("getDate",&ReceivedObject::getDate)
        .define_method("getId",&ReceivedObject::getId)
        .define_method("getVia",&ReceivedObject::getVia)
        .define_method("to_s",&ReceivedObject::toString);

    rb_cFipaBaseEnvelope = define_class_under<ACLBaseEnvelope>(rb_mFIPA, "ACLBaseEnvelope")
        .define_constructor(Constructor<ACLBaseEnvelope>())
        .define_method("getTo", &ACLBaseEnvelope::getTo)
        .define_method("clearAllTo", &ACLBaseEnvelope::clearAllTo)
        .define_method("getFrom", &ACLBaseEnvelope::getFrom)
        .define_method("getComments", &ACLBaseEnvelope::getComments)
        .define_method("getACLRepresentation", &ACLBaseEnvelope::getACLRepresentation)
        .define_method("getPayloadLength", &ACLBaseEnvelope::getPayloadLength)
        .define_method("getPayloadEncoding", &ACLBaseEnvelope::getPayloadEncoding)
        .define_method("getDate", &ACLBaseEnvelope::getDate)
        .define_method("getIntendedReceivers", &ACLBaseEnvelope::getIntendedReceivers)
        .define_method("clearAllIntendedReceivers", &ACLBaseEnvelope::clearAllIntendedReceivers)
        .define_method("getReceivedObject", &ACLBaseEnvelope::getReceivedObject)
        .define_method("hasReceivedObject", &ACLBaseEnvelope::hasReceivedObject)
        .define_method("getTransportBehaviour", &ACLBaseEnvelope::getTransportBehaviour)
        .define_method("setTo", &wrap_ACLBaseEnvelope_setTo, (Arg("agentids")) )
        .define_method("setFrom", &ACLBaseEnvelope::setFrom, (Arg("agentid")) )
        //.define_method("setComments", &ACLBaseEnvelope::getComments)
        //.define_method("setACLRepresentation", &ACLBaseEnvelope::setACLRepresentation, (Arg("representation")))
        //.define_method("setPayloadLength", &ACLBaseEnvelope::setPayloadLength, (Arg("payload-length")))
        //.define_method("setPayloadEncoding", &ACLBaseEnvelope::setPayloadEncoding, (Arg("encoding")))
        .define_method("setDate", &wrap_ACLBaseEnvelope_setDate, (Arg("time")))
        //.define_method("setIntendedReceivers", &ACLBaseEnvelope::setIntendedReceivers)
        //.define_method("getTransportBehaviour", &ACLBaseEnvelope::getTransportBehaviour)
        ;


    rb_cFipaEnvelope = define_class_under<ACLEnvelope>(rb_mFIPA, "ACLEnvelope")
        .define_constructor(Constructor<ACLEnvelope>())
   // RICE has a limitation of one constructor only, thus we use the insert method
   //.define_constructor(Constructor<ACLEnvelope, const ACLMessage&, const representation::Type&>())
        .define_method("insert", &ACLEnvelope::insert, (Arg("message"), Arg("representation")))
        .define_method("getDeliveryPath", &ACLEnvelope::getDeliveryPath)
        .define_method("getBaseEnvelope", &ACLEnvelope::getBaseEnvelope)
        .define_method("getExtraEnvelopes", &wrap_ACLEnvelope_getExtraEnvelopes)
        .define_method("getACLMessage", &ACLEnvelope::getACLMessage)
        .define_method("getTo", &wrap_ACLEnvelope_getTo)
        .define_method("getFrom", &wrap_ACLEnvelope_getFrom)
        .define_method("flattened", &ACLEnvelope::flattened)
        .define_method("to_byte_array", &wrap_ACLEnvelope_toByteVector)
        .define_method("from_byte_string", &wrap_ACLEnvelope_fromByteString)
        .define_method("addExtraEnvelope", &ACLEnvelope::addExtraEnvelope, (Arg("envelope")))
        .define_method("stamp", &ACLEnvelope::stamp, (Arg("agent_id")))
        .define_method("hasStamp?", &ACLEnvelope::hasStamp, (Arg("agent_id")))
        ;

    rb_cFipaMessage = define_class_under<ACLMessage>(rb_mFIPA, "ACLMessage")
        .define_constructor(Constructor<ACLMessage>())
        .define_method("setPerformative", &wrap_ACLMessage_setPerformative)
        .define_method("getPerformative", &wrap_ACLMessage_getPerformative)
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
        .define_method("getReplyBy", &ACLMessage::getReplyBy)
        .define_method("setConversationID", &ACLMessage::setConversationID, Arg("conversation_id"))
        .define_method("getConversationID", &ACLMessage::getConversationID)
        .define_method("to_byte_array", &wrap_ACLMessage_toByteVector)
        .define_method("from_byte_string", &wrap_ACLMessage_fromByteString)
        .define_method("to_s", &ACLMessage::toString)
        .define_method("setReplyBy",&wrap_ACLMessage_setReplyBy, Arg("time"))
        //.define_method("addUserDefinedParameter", &ACLMessage::addUserdefParam)
        //.define_method("getUserDefinedParameters", &wrap_getUserDefinedParameters)
        ;


    rb_cConversationMonitor = define_class_under<ConversationMonitor>(rb_mFIPA, "ConversationMonitor")
        .define_constructor(Constructor<ConversationMonitor, const AgentID&>(), Arg("name"))
        .define_method("updateConversation", &ConversationMonitor::updateConversation, Arg("message")) // ConversationPtr
        .define_method("startConversation", &ConversationMonitor::startConversation, Arg("conversationID/topic")) //ConversationPtr
        .define_method("removeConversation", &ConversationMonitor::removeConversation, Arg("conversationID"))
        .define_method("getConversation", &ConversationMonitor::getConversation, Arg("conversationID")) //ConversationPtr
        .define_method("getOrCreateConversation", &ConversationMonitor::getOrCreateConversation, Arg("conversationID"))
        .define_method("cleanup",&ConversationMonitor::cleanup)   // ConversationPtr
        .define_method("getActiveConversations",&ConversationMonitor::getActiveConversations); // ConversationPtr

    rb_cConversation = define_class_under<Conversation>(rb_mFIPA, "Conversation")
        .define_method("getProtocol", &Conversation::getProtocol)
        .define_method("getConversationID", &wrap_ConversationMonitor_getConversationId)
        ;

}
