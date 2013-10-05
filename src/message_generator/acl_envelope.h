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

class ACLBaseEnvelope;
typedef std::vector<ACLBaseEnvelope> ACLBaseEnvelopeList;

namespace envelope {

    // Parameter id to allow checking whether certain field in an envelope
    // have been set
    enum ParameterId { NONE = 0x00, TO = 0x01, FROM = 0x02, COMMENTS = 0x04,
ACL_REPRESENTATION = 0x08, PAYLOAD_LENGTH = 0x10, PAYLOAD_ENCODING = 0x20, DATE = 0x40, INTENDED_RECEIVERS = 0x80, RECEIVED_OBJECT = 0x0100, TRANSPORT_BEHAVIOUR = 0x0200, USERDEFINED_PARAMETERS = 0x0400, PARAMETER_END = 0x0800};
}


class ACLBaseEnvelope
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
     * The adding of the parameter is done by adding an additional envelope with the corresponding parameter set
     * Optional field
     */
    ReceivedObject mReceivedObject;

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
    ACLBaseEnvelope();

    /**
     + Test wether a certain content element is set in this envelope object
     *
     */
    bool contains(envelope::ParameterId id) const { return mParameters & id; }

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
     * Get the ACLRepresentation in use
     * \return acl representation
     */
    const std::string& getACLRepresentationString() const { return representation::TypeTxt[mACLRepresentation]; }

    /**
     * Set the acl representation in use
     * "This is the name of the syntax representation of the message payload." [http://www.fipa.org/specs/fipa00067/SC00067F.html#_Toc26669814]
     */
    void setACLRepresentation(representation::Type representation);

    /**
     * Set ACLRepresentation from string as defined in standard specification
     * \throws if the ACLRepresentation in unknown to the system
     */
    void setACLRepresentation(const std::string& represention);

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
     * Get the associated received object 
     */
    const ReceivedObject& getReceivedObject() const { return mReceivedObject; }

    /**
     * Set the received object list
     */
    void setReceivedObject(const ReceivedObject& receivedObject);

    /**
     * Test whether the received object has the same sender
     * \return true if the By field is the same, false otherwise
     */
    bool hasReceivedObject(const ReceivedObject& receivedObject) const;

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
    ACLBaseEnvelope merge(const ACLBaseEnvelope& other) const;

    /**
     * Flatten the current envelope plus the extra envelopes
     * (the most current/latest has to be at the end of the list
     * \param list of overlay envelopes
     * \return the most current ACLBaseEnvelope representation 
     */
    ACLBaseEnvelope flatten(const ACLBaseEnvelopeList& extraEnvelopes) const;

    /**
     * Add a receipt for this message, i.e. add a received object to the envelope. This function is an alias for setReceivedObject
     */
    void stamp(const ReceivedObject& receivedObject) { setReceivedObject(receivedObject); }

    /**
     * Test whether this envelope has been stamped. This is an alias for hasReceivedObject
     * \return true if the sender has already stamped this message
     */
    bool hasStamp(const ReceivedObject& receivedObject) const { return hasReceivedObject(receivedObject); }
};


class EnvelopeFormat;
/**
 * Message envelope includes the base envelope and updated fields, along with the actual payload
 * data that is wrapped by this envelope
 */
class ACLEnvelope
{
    friend class EnvelopeFormat;

    /**
     * Updated envelope information as part of the relaying through various
     * message transport services
     */
    ACLBaseEnvelopeList mExtraEnvelopes;

    /**
     * Base envelope, i.e. the one the first mts created 
     */
    ACLBaseEnvelope mBaseEnvelope;

    // The payload data that is transported within this envelope
    std::string mPayload;

    /**
     * Count of stamps
     */
    static uint32_t mStampCounter;

    /**
     * Create an id for the received object
     * <counter>:<timestamp>
     */
    static ID createLocalId();

public:

    /**
     * Default envelope constructor
     */
    ACLEnvelope();

    /**
     * Creates an envelope for an acl message.
     * Note the the acl message will be encoded according to the given representation format
     *
     * Note, that the fields to, from, intended-receivers, acl-representation, payload-length, payload-encoding and date are set. All others have to be set explicitly (also see stamp).
     * Intended-receivers is initially same as 'to' and should only be modified by a transport service to organize message delivery
     * \param msg ACLMessage will shall be wrapped by this envelope
     * \param format The message format this message will be encoded in for transport
     */
    ACLEnvelope(const ACLMessage& msg, const fipa::acl::representation::Type& format);

    /**
     * Insert a message to this envelope using a specific representation
     * Existing settings will be overwritten
     * \param msg Message to be set
     * \param format Format to be applied to the message
     */
    void insert(const ACLMessage& msg, const fipa::acl::representation::Type& format);

    /**
     * Get all extra envelopes.
     * Extra envelope represent an overlay to the base envelope,
     * This overlay mechanism is used, since information must not be discarded.
     *
     * \return extra envelopes
     */
    const ACLBaseEnvelopeList& getExtraEnvelopes() const { return mExtraEnvelopes; }

    /**
     * Add an extra envelope to overwrite existing parameters
     * Append the most recent extra envelope
     */
    void addExtraEnvelope(const ACLBaseEnvelope& envelope)  { mExtraEnvelopes.push_back(envelope); }

    /**
     * Set all extra envelopes
     * Overwrites the existing set of extra envelopes
     * \oaram List of base envelopes
     */
    void setExtraEnvelopes(const ACLBaseEnvelopeList& envelopes) { mExtraEnvelopes = envelopes; }

    /**
     * Get the base envelope
     * \return base envelope
     */
    const ACLBaseEnvelope& getBaseEnvelope() const { return mBaseEnvelope; }

    /**
     * Set the base envelope
     * \param envelope Base envelope to set
     */
    void setBaseEnvelope(const ACLBaseEnvelope& envelope) { mBaseEnvelope = envelope; }

    /**
     * Retrieve the fully merged envelope
     * Starting with the base envelope each extra envelope is applied 
     * as an overlay
     * \return the envelope representing the most recent combination of information in base and extra envelopes
     */
    ACLBaseEnvelope flattened() const { return mBaseEnvelope.flatten(mExtraEnvelopes); }

    /**
     * Retrieve the delivery path from the 
     * received object in base and extra envelopes, i.e.
     * will not include the current hop (unless stamped)
     *
     * \return List of agents id representing the delivery path, in time order, i.e. oldest first to this agent
     */
    AgentIDList getDeliveryPath() const;

    /**
     * Retrieve the delivery path as a comma separated list
     * of agent names in time order
     * \return String representiong the delivery path
     */
    std::string getDeliveryPathString() const;

    /**
     * Set the payload which is wrapped by this envelope as byte get
     * If you are using this function, make sure to set the corresponding encoding of the payload
     * \param payload representing an acl message
     */
    void setPayload(const std::string& payload) { mPayload = payload; }

    /**
     * Get the payload which is wrapped by this envelope
     * \return string as byte container
     */
    const std::string& getPayload() const { return mPayload; }

    /**
     * Retrieve the contained message if possible. 
     * \return decoded payload
     * \throws std::runtime_error if the acl message could not be extracted
     */
    fipa::acl::ACLMessage getACLMessage() const;

    /**
     * Stamp a letter with a given agent id
     * This creates a new received object and attaches it to 
     * an extra envelope
     *
     * The mandatory fields 'by' and 'date' are set.
     * The optional field 'id' is set.
     */
    void stamp(const fipa::acl::AgentID& id);

    /**
     * Check whether the letter has already been stamped 
     * Iterates over all received objects for testing
     * \return True, if the agent id is set in one of the received objects, false otherwise
     */
    bool hasStamp(fipa::acl::AgentID id) const;
};

// We also define a letter to be the envelope and the payload
typedef ACLEnvelope Letter;

} // end namespace acl
} // end namespace fipa
#endif // FIPA_ACL_MESSAGE_ENVELOPE_H
