#ifndef FIPA_ACL_MESSAGE_ENVELOPE_H
#define FIPA_ACL_MESSAGE_ENVELOPE_H

#include <stdint.h>
#include <string>
#include <vector>
#include <base/time.h>
#include <fipa_acl/message_generator/userdef_param.h>
#include <fipa_acl/message_generator/agent_id.h>

namespace fipa {
namespace acl {

typedef std::string Comments;
typedef uint32_t PayloadLength;
typedef std::string PayloadEncoding;
typedef std::string URL;
typedef std::string ID;
typedef std::string Via;
typedef std::string ACLRepresentation;
typedef std::string TransportBehaviour;

/**
 * Received object is attached to the message envelope by each Message Transport Service
 */
class ReceivedObject
{
private: 
    /**
     * The URL representing the transport address of the receiving ACC.
     */
    URL mBy;

    /**
     *  The URL representing the transport address of the sending ACC.
     */
    URL mFrom;

    /**
     * The date when a message was received.
     */
    base::Time mDate;
    
    /**
     *  The unique identifier of a message. It is required that uniqueness be guaranteed within the scope of the sending ACC only.
     */
    ID mId;

    /**
     *  The type of MTP the message was delivered over.
     */
    Via mVia;

    /**
     * Additional parameters users can add
     */
    std::vector<UserdefParam> mUserdefinedParameters;

public:

    /**
     * Get URL representing the transport address of the receiving ACC
     */
    const URL& getBy() { return mBy; }

    /**
     * Set URL representing the transport address of the receiving ACC
     */
    void setBy(const URL& by) { mBy = by; }

    /**
     * Get URL representing the transport address of the sending ACC
     */
    const URL& getFrom() { return mFrom; }

    /**
     * Set URL representing the transport address of the sending ACC
     */
    void setFrom(const URL& from) { mFrom = from; }

    /**
     * Get the date when the message was received
     */
    const base::Time& getDate() { return mDate; }

    /**
     * Set the date when the message was received
     */
    void setDate(const base::Time& date) { mDate = date; }

    /**
     * Get the unique id of the message - unique within the scope of the sending ACC
     */
    const ID& getId() { return mId; }

    /**
     * Set the unique id of the message - unique within the scope of the sending ACC
     */
    void setId(const ID& id) { mId = id; }

    /**
     * Get the via - the MTP the message was delivered over
     */
    const Via& getVia() { return mVia; }

    /**
     * Set via
     */
    void setVia(const Via& via) { mVia = via; }

    /**
     * Get userdefined parameters
     */ 
    const std::vector<UserdefParam>& getUserdefinedParameters() { return mUserdefinedParameters; }

    /**
     * Set userdefined parameters
     */
    void setUserdefinedParameters(const std::vector<UserdefParam>& paramList) { mUserdefinedParameters = paramList; }

};

class ACLMessageEnvelope
{
private:
    /**
     * If no intended-receiver parameter is present, then the information in this parameter is used to generate intended-receiver field for the messages the ACC subsequently forwards. 
     */
    AgentIDList mTo;

    /**
     * If required, the ACC returns error and confirmation messages to the agent specified in this parameter.
     */
    AgentID mFrom;

    /**
     * This information is intended for the final recipient of the message. 
     */
    Comments mComments;

    /**
     * Representation
     */
    ACLRepresentation mACLRepresentation;

    /**
     * The ACC may use this information to improve parsing efficiency.
     */
    PayloadLength mPayloadLength;

    /**
     * This information is intended for the final recipient of the message.
     */
    PayloadEncoding mPayloadEncoding;

    /**
     * This information is intended for the final recipient of the message.
     */
    base::Time mDate;

    /**
     * An ACC uses this parameter to determine where this instance of a message should be sent.
     * If this parameter is not provided, then the first ACC to receive the message should generate an intended-receiver parameter using the to parameter.
     */
    AgentIDList mIntendedReceiver;

    /**
     *  A new received parameter is added to the envelope by each ACC that the message passes through. 
     *  Each ACC handling a message must add a completed received parameter. 
     *  If an ACC receives a message it has already stamped, it is free to discard the message without any need to generate an error message.
     */
    ReceivedObject mReceivedObject; 

    /**
     * Reserved for future use.
     */
    TransportBehaviour mTransportBehaviour;

public: 
    /**
     * Get destination
     * \return destination of this message envelope as list of agents
     */
    const AgentIDList& getTo() { return mTo; }

    /**
     * Set destination
     * \param receivers receiver list
     */
    void setTo(const AgentIDList& receivers) { mTo = receivers; }

    /**
     * Get sender id 
     * \return agent id of sender
     */
    const AgentID& getFrom() { return mFrom; }

    /**
     * Set sender id
     * \param from
     */
    void setFrom(const AgentID& from) { mFrom = from; }

    /**
     * Get comments
     * \return comments
     */
    const Comments& getComments() { return mComments; }

    /**
     * Set comments
     * \param comments Comments to be set
     */
    void setComments(const Comments& comments) { mComments = comments; }

    /**
     * Get the ACLRepresentation in use
     * \return acl representation
     */
    const ACLRepresentation& getACLRepresentation() { return mACLRepresentation; }

    /**
     * Set the acl representation in use
     */
    void setACLRepresentation(const ACLRepresentation& representation) { mACLRepresentation = representation; }

    /**
     * Get the length of the payload
     */
    PayloadLength getPayloadLength() { return mPayloadLength; }

    /**
     * Set the length of the payload
     */
    void setPayloadLength(const PayloadLength& length) { mPayloadLength = length; }

    /**
     * Get the payload encoding
     */
    PayloadEncoding getPayloadEncoding() { return mPayloadEncoding; }

    /**
     * Set the payload encoding
     */
    void setPayloadEncoding(const PayloadEncoding& encoding) { mPayloadEncoding = encoding; }

    /**
     * Retrieve date
     */
    const base::Time& getDate() { return mDate; }

    /**
     * Set date
     */
    void setDate(const base::Time& date) { mDate = date; }

    /**
     * Get intended receivers
     */
    AgentIDList getIntendedReceiver() { return mIntendedReceiver; }

    /**
     * Set intended receivers
     */
    void setIntendedReceiver(const AgentIDList& receiver) { mIntendedReceiver = receiver; }

    /**
     * Get received object
     */
    const ReceivedObject& getReceivedObject() { return mReceivedObject; }

    /**
     * Set receiver object
     */
    void setReceivedObject(const ReceivedObject& receivedObject) { mReceivedObject = receivedObject; }

    /**
     * Get the transport behaviour
     */
    const TransportBehaviour& getTransportBehaviour() { return mTransportBehaviour; }

    /**
     * Set the transport behaviour
     */
    void setTransportBehaviour(const TransportBehaviour& transportBehaviour) { mTransportBehaviour = transportBehaviour; }
};

} // end namespace acl
} // end namespace fipa
#endif // FIPA_ACL_MESSAGE_ENVELOPE_H
