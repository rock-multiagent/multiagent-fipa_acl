#ifndef FIPA_ACL_RECEIVED_OBJECT_H
#define FIPA_ACL_RECEIVED_OBJECT_H

#include <string>
#include <vector>
#include <base/Time.hpp>
#include "userdef_param.h"

namespace fipa {
namespace acl {

typedef std::string URL;
typedef std::string ID;
typedef std::string Via;

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
     *
     */
    bool operator==(const ReceivedObject& receivedObject) const { return true; }

    /**
     * Get URL representing the transport address of the receiving ACC
     */
    const URL& getBy() const { return mBy; }

    /**
     * Set URL representing the transport address of the receiving ACC
     */
    void setBy(const URL& by) { mBy = by; }

    /**
     * Get URL representing the transport address of the sending ACC
     */
    const URL& getFrom() const { return mFrom; }

    /**
     * Set URL representing the transport address of the sending ACC
     */
    void setFrom(const URL& from) { mFrom = from; }

    /**
     * Get the date when the message was received
     */
    const base::Time& getDate() const { return mDate; }

    /**
     * Set the date when the message was received
     */
    void setDate(const base::Time& date) { mDate = date; }

    /**
     * Get the unique id of the message - unique within the scope of the sending ACC
     */
    const ID& getId() const { return mId; }

    /**
     * Set the unique id of the message - unique within the scope of the sending ACC
     */
    void setId(const ID& id) { mId = id; }

    /**
     * Get the via - the MTP the message was delivered over
     */
    const Via& getVia() const { return mVia; }

    /**
     * Set via
     */
    void setVia(const Via& via) { mVia = via; }

    /**
     * Create to string methods
     */
    std::string toString(size_t indent = 0) const;

    /**
     * Get userdefined parameters
     */
    const UserdefinedParameterList& getUserdefinedParameters() const { return mUserdefinedParameters; }

    /**
     * Set userdefined parameters
     */
    void setUserdefinedParameters(const UserdefinedParameterList& paramList) { mUserdefinedParameters = paramList; }
};


} // end namespace acl
} // end namespace fipa

#endif // FIPA_ACL_RECEIVED_OBJECT_H
