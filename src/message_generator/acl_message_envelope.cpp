#include "acl_message_envelope.h"

#include <algorithm>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <fipa_acl/message_parser/message_parser.h>
#include <fipa_acl/message_generator/message_generator.h>

using namespace boost::phoenix;
using namespace boost::phoenix::arg_names;
using namespace std;
using namespace fipa::acl::envelope;

namespace fipa {
namespace acl {

ACLBaseMessageEnvelope::ACLBaseMessageEnvelope()
    : mParameters(NONE)
    , mACLRepresentation(representation::UNKNOWN)
    , mPayloadEncoding("US-ASCII")
{}

void ACLBaseMessageEnvelope::addTo(const AgentID& agentId)
{
    AgentID agent = agentId;
    AgentIDList::const_iterator it = find_if(mTo.begin(), mTo.end(), arg1 == agentId );
    if(it == mTo.end())
    {
        mTo.push_back(agentId);
    }
}

void ACLBaseMessageEnvelope::setTo(const AgentIDList& receivers)
{
    mParameters = TO;
    mTo = receivers;
}

bool ACLBaseMessageEnvelope::removeTo(const AgentID& agentId)
{
    AgentIDList::iterator it = find_if(mTo.begin(), mTo.end(), arg1 == agentId);
    if(it != mTo.end())
    {
        mTo.erase(it);
        return true;
    }
    return false;
}


void ACLBaseMessageEnvelope::setFrom(const AgentID& from)
{
    mParameters = (ParameterId) (mParameters | FROM);
    mFrom = from;
}


void ACLBaseMessageEnvelope::setComments(const Comments& comments)
{
    mParameters = (ParameterId) (mParameters | COMMENTS);
    mComments = comments;
}

void ACLBaseMessageEnvelope::setACLRepresentation(representation::Type representation)
{
    mParameters = (ParameterId) (mParameters | ACL_REPRESENTATION);
    mACLRepresentation = representation;
}

void ACLBaseMessageEnvelope::setPayloadLength(const PayloadLength& length)
{
    mParameters = (ParameterId) (mParameters | PAYLOAD_LENGTH);
    mPayloadLength = length;
}

void ACLBaseMessageEnvelope::setPayloadEncoding(const PayloadEncoding& encoding)
{
    mParameters = (ParameterId) (mParameters | PAYLOAD_ENCODING);
    mPayloadEncoding = encoding;
}

void ACLBaseMessageEnvelope::setDate(const base::Time& date)
{
    mParameters = (ParameterId) (mParameters | DATE);
    mDate = date;
}

void ACLBaseMessageEnvelope::setIntendedReceivers(const AgentIDList& receivers)
{
    mParameters = (ParameterId) (mParameters | INTENDED_RECEIVERS);
    mIntendedReceivers = receivers;
}

void ACLBaseMessageEnvelope::addIntendedReceiver(const AgentID& agentId)
{
     AgentIDList::const_iterator it = find_if(mIntendedReceivers.begin(), mIntendedReceivers.end(), arg1 == agentId);
     if(it == mIntendedReceivers.end())
     {
         mIntendedReceivers.push_back(agentId);
     }
}

bool ACLBaseMessageEnvelope::removeIntendedReceiver(const AgentID& agentId)
{
     AgentIDList::iterator it = find_if(mIntendedReceivers.begin(), mIntendedReceivers.end(), arg1 == agentId);
     if(it != mIntendedReceivers.end())
     {
         mIntendedReceivers.erase(it);
         return true;
     }
     return false;
}

bool ACLBaseMessageEnvelope::hasReceivedObject(const ReceivedObject& receivedObject) const
{
    std::vector<ReceivedObject>::const_iterator it = find_if(mReceivedObjects.begin(), mReceivedObjects.end(), arg1 == receivedObject);
    if(it != mReceivedObjects.end())
    {
        return true;
    }

    return false;
}

void ACLBaseMessageEnvelope::addReceivedObject(const ReceivedObject& receivedObject)
{
    mParameters = (ParameterId) (mParameters | RECEIVED_OBJECTS);
    if(!hasReceivedObject(receivedObject))
    {
        mReceivedObjects.push_back(receivedObject);
    }
}

void ACLBaseMessageEnvelope::setReceivedObjects(const ReceivedObjectList& receivedObjects)
{
    mParameters = (ParameterId) (mParameters | RECEIVED_OBJECTS);
    mReceivedObjects = receivedObjects;
}

void ACLBaseMessageEnvelope::setTransportBehaviour(const TransportBehaviour& transportBehaviour)
{
    mParameters = (ParameterId) (mParameters | TRANSPORT_BEHAVIOUR);
    mTransportBehaviour = transportBehaviour;
}

void ACLBaseMessageEnvelope::setUserdefinedParameters(const UserdefinedParameterList& parameters)
{
    mParameters = (ParameterId) (mParameters | USERDEFINED_PARAMETERS);
    mUserdefinedParameters = parameters;
}

ACLBaseMessageEnvelope ACLBaseMessageEnvelope::merge(const ACLBaseMessageEnvelope& other) const
{
    ACLBaseMessageEnvelope envelope(*this);
    if(!envelope.contains(TO))
    {
        envelope.setTo(other.getTo());
    }

    if(!envelope.contains(FROM))
    {
        envelope.setFrom(other.getFrom());
    }

    if(!envelope.contains(COMMENTS))
    {
        envelope.setComments(other.getComments());
    }

    if(!envelope.contains(ACL_REPRESENTATION))
    {
        envelope.setACLRepresentation(other.getACLRepresentation());
    }

    if(!envelope.contains(PAYLOAD_LENGTH))
    {
        envelope.setPayloadLength(other.getPayloadLength());
    }

    if(!envelope.contains(PAYLOAD_ENCODING))
    {
        envelope.setPayloadEncoding(other.getPayloadEncoding());
    }

    if(!envelope.contains(PAYLOAD_ENCODING))
    {
        envelope.setDate(other.getDate());
    }

    if(!envelope.contains(INTENDED_RECEIVERS))
    {
        envelope.setIntendedReceivers(other.getIntendedReceivers());
    }

    if(!envelope.contains(RECEIVED_OBJECTS))
    {
        envelope.setReceivedObjects(other.getReceivedObjects());
    }

    if(!envelope.contains(TRANSPORT_BEHAVIOUR))
    {
        envelope.setTransportBehaviour(other.getTransportBehaviour());
    }

    if(!envelope.contains(USERDEFINED_PARAMETERS))
    {
        envelope.setUserdefinedParameters(other.getUserdefinedParameters());
    }

    return envelope;
}

ACLMessageEnvelope::ACLMessageEnvelope()
{}

ACLMessageEnvelope::ACLMessageEnvelope(const fipa::acl::ACLMessage& message, const fipa::acl::representation::Type& representation)
{
    using namespace fipa::acl;
    mPayload = MessageGenerator::create(message, representation);
}

fipa::acl::ACLMessage ACLMessageEnvelope::getACLMessage() const
{
    using namespace fipa::acl;
    ACLMessage msg;
    MessageParser mp;
    mp.parseData(mPayload, msg);
    return msg;
}

void ACLMessageEnvelope::stamp(const fipa::acl::AgentID& id)
{
    ReceivedObject receivedObject;

    // Mandatory fields
    receivedObject.setBy(id.getName());
    receivedObject.setDate(base::Time::now());

    mBaseEnvelope.stamp(receivedObject);
}

bool ACLMessageEnvelope::hasStamp(const fipa::acl::AgentID id) const
{
    ReceivedObject receivedObject;
    receivedObject.setBy(id.getName());

    return mBaseEnvelope.hasStamp(receivedObject);
}


} // end namespace acl
} // end namespace fipa
