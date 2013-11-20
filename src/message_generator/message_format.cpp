#include "message_format.h"
#include <boost/assign/list_of.hpp>

namespace fipa {
namespace acl {
    
MessageFormat::MessageFormat()
    : mUseCodeTables(false)
    , mUpdateCodeTables(true)
    , mVersion("1.3")
    , mResolverDepth(1)
{
}

std::map<MessageField::Type, std::string> MessageField::MessageFieldTxt = boost::assign::map_list_of
    (MessageField::SENDER, "sender")
    (MessageField::RECEIVER, "receiver")
    (MessageField::CONTENT, "content")
    (MessageField::REPLY_WITH, "reply-with")
    (MessageField::REPLY_BY, "reply-by")
    (MessageField::IN_REPLY_TO, "in-reply-to")
    (MessageField::REPLY_TO, "reply-to")
    (MessageField::LANGUAGE, "language")
    (MessageField::ENCODING, "encoding")
    (MessageField::ONTOLOGY, "ontology")
    (MessageField::PROTOCOL, "protocol")
    (MessageField::CONVERSATION_ID, "conversation-id");

}//end of acl namespace
}// end of fipa namespace

