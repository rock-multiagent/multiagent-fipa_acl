#ifndef FIPA_ACL_MESSAGE_ENVELOPE_H
#define FIPA_ACL_MESSAGE_ENVELOPE_H

#include <stdint.h>
#include <fipa_acl/message_generator/acl_message.h>
#include <fipa_acl/message_generator/received_object.h>
#include <fipa_acl/message_generator/types.h>

namespace fipa {
namespace acl {

typedef std::string Comments;
typedef uint32_t PayloadLength;
typedef std::string PayloadEncoding;
typedef std::string ACLRepresentation;
typedef std::string TransportBehaviour;
typedef std::vector<ReceivedObject> ReceivedObjectList;

class ACLBaseMessageEnvelope;
typedef std::vector<ACLBaseMessageEnvelope> ACLBaseMessageEnvelopeList;

namespace envelope {

    enum ParameterId { NONE = 0x00, TO = 0x01, FROM = 0x02, COMMENTS = 0x04,
ACL_REPRESENTATION = 0x08, PAYLOAD_LENGTH = 0x10, PAYLOAD_ENCODING = 0x20, DATE = 0x40, INTENDED_RECEIVERS = 0x80, RECEIVED_OBJECTS = 0x0100, TRANSPORT_BEHAVIOUR = 0x0200, USERDEFINED_PARAMETERS = 0x0400, PARAMETER_END = 0x0800};
}
namespace representation {
    enum Type { UNKNOWN, BITEFFICIENT, STRING, XML };

    const std::string TypeTxt[] = { "unknown", "fipa.acl.rep.bitefficient.std", "fipa.acl.rep.string.std", "fipa.acl.rep.xml.std"};
}

class ACLBaseMessageEnvelope
{
private:
    envelope::ParameterId mParameters;

    /**
     * If no intended-receiver parameter is present, then the information in this parameter is used to generate intended-receiver field for the messages the ACC subsequently forwards.
     *
     * Mandatory field
     */
    AgentIDList mTo;

    /**
     * If required, the ACC returns error and confirmation messages to the agent specified in this parameter.
     *
     * Mandatory field
     */
    AgentID mFrom;

    /**
     * This information is intended for the final recipient of the message.
     *
     * Optional field

     */
    Comments mComments;

    /**
     * Representation
     *
     * Mandatory field
     */
    representation::Type mACLRepresentation;

    /**
     * The ACC may use this information to improve parsing efficiency.
     *
     * Optional field
     */
    PayloadLength mPayloadLength;

    /**
     * This information is intended for the final recipient of the message.
     *
     * Optional field
     */
    PayloadEncoding mPayloadEncoding;

    /**
     * This information is intended for the final recipient of the message.
     *
     * Mandatory field
     */
    base::Time mDate;

    /**
     * An ACC uses this parameter to determine where this instance of a message should be sent.
     * If this parameter is not provided, then the first ACC to receive the message should generate an intended-receiver parameter using the to parameter.
     * Remarks in spec: http://www.fipa.org/specs/fipa00067/SC00067F.html
     * If an ACC receives a message envelope without an intended-receiver parameter, then it generates a new intended-receiver parameter from the to parameter (possibly containing multiple AIDs).
     * It may also generate multiple copies of the message with different intended-receiver parameters
     * if multiple receivers are specified. In all cases, the ACC is required to process all entries in
     * the to field parameter and enforced not to add and not to remove any AID that was contained in
     * the original message. The intended-receiver parameters form a delivery path showing the route that
     * a message has taken.
     *
     * If an ACC receives a message envelope with an intended-receiver parameter, this is used for delivery of this instance of the message and the to parameter is ignored.
     *
     * If an ACC receives a message envelope with more than one intended-receiver parameter, the most recent is used.
     *
     * Optional field
     */
    AgentIDList mIntendedReceivers;

    /**
     * A new received parameter is added to the envelope by each ACC that the message passes through.
     * Each ACC handling a message must add a completed received parameter.
     * If an ACC receives a message it has already stamped, it is free to discard the message without any need to generate an error message.
     *
     * Before forwarding the message, the ACC adds a completed received parameter to the message envelope. Once an ACC has forwarded a message it no longer needs to keep any record of the existence of that message.
     *
     * Optional field
     */
    ReceivedObjectList mReceivedObjects;

    /**
     * Reserved for future use.
     *
     * Optional field
     */
    TransportBehaviour mTransportBehaviour;

    /**
     * Userdefined parameter than can be set for an envelope
     * keys should start with 'X-'
     */
    UserdefinedParameterList mUserdefinedParameters;

public:
    /**
     * DefaultConstructor
     */
    ACLBaseMessageEnvelope();

    /**
     + Test wether a certain content element is set in this envelope object
     *
     */
    bool contains(envelope::ParameterId id) { return mParameters & id; }

    /**
     * Get destination
     * \return destination of this message envelope as list of agents
     */
    const AgentIDList& getTo() const { return mTo; }

    /**
     * Add an agent identifier to the destination list
     */
    void addTo(const AgentID& agent);

    /**
     * Set destination
     * \param receivers receiver list
     */
    void setTo(const AgentIDList& receivers);

    /**
     * Remove an agents from the 'to' list
     * \return true upon success, i.e. if the agent was is this list
     * false otherwise
     */
    bool removeTo(const AgentID& agentId);

    /**
     * Clear 'to' list
     */
    void clearAllTo() { mTo.clear(); }

    /**
     * Get sender id
     * \return agent id of sender
     */
    const AgentID& getFrom() const { return mFrom; }

    /**
     * Set sender id
     * \param from
     */
    void setFrom(const AgentID& from);

    /**
     * Get comments
     * \return comments
     */
    const Comments& getComments() const { return mComments; }

    /**
     * Set comments
     * \param comments Comments to be set
     */
    void setComments(const Comments& comments);

    /**
     * Get the ACLRepresentation in use
     * \return acl representation
     */
    const representation::Type& getACLRepresentation() const { return mACLRepresentation; }

    /**
     * Set the acl representation in use
     * "This is the name of the syntax representation of the message payload." [http://www.fipa.org/specs/fipa00067/SC00067F.html#_Toc26669814]
     */
    void setACLRepresentation(representation::Type representation);

    /**
     * Get the length of the payload
     */
    const PayloadLength& getPayloadLength() const { return mPayloadLength; }

    /**
     * Set the length of the payload
     */
    void setPayloadLength(const PayloadLength& length);

    /**
     * Get the payload encoding (default is US-ASCII)
     */
    const PayloadEncoding& getPayloadEncoding() const { return mPayloadEncoding; }

    /**
     * Set the payload encoding (default is US-ASCII)
     */
    void setPayloadEncoding(const PayloadEncoding& encoding);

    /**
     * Retrieve creation date and time of the message envelope
     */
    const base::Time& getDate() const { return mDate; }

    /**
     * Set creation date and time of the message envelope
     */
    void setDate(const base::Time& date);

    /**
     * Get intended receivers
     */
    const AgentIDList& getIntendedReceivers() const { return mIntendedReceivers; }

    /**
     * Set intended receivers
     */
    void setIntendedReceivers(const AgentIDList& receivers);

    /**
     * Add intended receiver to the list of intended receivers
     */
    void addIntendedReceiver(const AgentID& agent);

    /**
     * Remove an intended receiver from this list of intended receivers
     */
    bool removeIntendedReceiver(const AgentID& agent);

    /**
     * Clear list of intended receivers
     */
    void clearAllIntendedReceivers() { mIntendedReceivers.clear(); }

    /**
     * Get all received objects
     */
    const ReceivedObjectList& getReceivedObjects() const { return mReceivedObjects; }

    /**
     * Has received object test whether the list of received object
     * contains an object of the same sender / By-Field
     */
    bool hasReceivedObject(const ReceivedObject& receivedObject) const;

    /**
     * Add received object
     */
    void addReceivedObject(const ReceivedObject& receivedObject);

    /**
     * Set the received object list
     */
    void setReceivedObjects(const ReceivedObjectList& receivedObjects);

    /**
     * Get the transport behaviour
     */
    const TransportBehaviour& getTransportBehaviour() const { return mTransportBehaviour; }

    /**
     * Set the transport behaviour
     */
    void setTransportBehaviour(const TransportBehaviour& transportBehaviour);

    /**
     * Get userdefined parameter list
     */
    const UserdefinedParameterList& getUserdefinedParameters() const { return mUserdefinedParameters; }

    /**
     * Set userdefined parameters
     */
    void setUserdefinedParameters(const UserdefinedParameterList& parameters);

    /**
     * Merges the information of two envelopes -- only empty field in this will be overwritten by other
     */
    ACLBaseMessageEnvelope merge(const ACLBaseMessageEnvelope& other) const;

    /**

    /**
     * Add a receipt for this message, i.e. add a received object to the envelope. This function is an alias for setReceivedObject
     */
    void stamp(const ReceivedObject& receivedObject) { addReceivedObject(receivedObject); }


    /**
     * Test whether this envelope has been stamped. This is an alias for hasReceivedObject
     * \return true if the sender has already stamped this message
     */
    bool hasStamp(const ReceivedObject& receivedObject) const { return hasReceivedObject(receivedObject); }
};


/**
 * Message envelope includes the base envelope and updated fields, along with the actual payload
 * data that is wrapped by this envelope
 */
class ACLMessageEnvelope
{
    /**
     * Updated envelope information as part of the relaying through various
     * message transport services
     */
    ACLBaseMessageEnvelopeList mExtraEnvelopes;

    /**
     * Base envelope, i.e. the one the first mts created 
     */
    ACLBaseMessageEnvelope mBaseEnvelope;

    // The payload data that is transported within this envelope
    std::vector<uint8_t> mPayload;

public:
    /**
     * Get all extra envelopes
     */
    const ACLBaseMessageEnvelopeList& getExtraEnvelopes() const { return mExtraEnvelopes; }

    /**
     * Set all extra envelopes
     */
    void setExtraEnvelopes(const ACLBaseMessageEnvelopeList& envelopes) { mExtraEnvelopes = envelopes; }

    /**
     * Get the base envelope
     */
    const ACLBaseMessageEnvelope& getBaseEnvelope() const { return mBaseEnvelope; }

    /**
     * Set the base envelope
     */
    void setBaseEnvelope(const ACLBaseMessageEnvelope& envelope) { mBaseEnvelope = envelope; }

    /**
     * Set the payload which is wrapped by this envelope as byte get
     */
    void setPayload(std::vector<uint8_t> payload) { mPayload = payload; }

    /**
     * Get the payload which is wrapped by this envelope
     * \return byte vector
     */
    std::vector<uint8_t> getPayload() const { return mPayload; }

    /**
     * Stamp a letter with a given agent id
     */
    void stamp(const fipa::acl::AgentID& id);

    /**
     * Check whether the letter has already been stamped 
     */
    bool hasStamp(fipa::acl::AgentID id) const;
};

// We also define a letter to be the envelope and the payload
typedef ACLMessageEnvelope Letter;

} // end namespace acl
} // end namespace fipa
#endif // FIPA_ACL_MESSAGE_ENVELOPE_H
