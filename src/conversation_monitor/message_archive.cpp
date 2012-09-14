#include "message_archive.h"

namespace fipa {
namespace acl {

const ACLMessage& MessageArchive::getInitiatingMessage() const
{
    return mMessages.front();
}

void MessageArchive::addMessage(const ACLMessage& msg) 
{
    mMessages.push_back(msg);
}

} // end namespace acl
} // end namespace fipa
