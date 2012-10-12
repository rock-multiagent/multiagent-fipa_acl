#include "message_generator.h"
#include <stdexcept>
#include <boost/assign/list_of.hpp>
#include <base/logging.h>

#include "format/bitefficient_format.h"

namespace fipa {
namespace acl {

std::map<message_format::Type, MessageFormatPtr > MessageGenerator::msFormats = boost::assign::map_list_of
    (message_format::BITEFFICIENT, boost::shared_ptr<MessageFormat>(new BitefficientFormat()) );

std::string MessageGenerator::create(const ACLMessage& msg, message_format::Type type)
{
    std::map<message_format::Type, MessageFormatPtr >::const_iterator it = msFormats.find(type);

    if( it != msFormats.end())
    {
        return it->second->apply(msg);
    } else
    {
        char buffer[512];
        snprintf(buffer, 512, "Message format of type '%d' is unknown", (int) type);
        LOG_ERROR("%s", buffer);
        throw std::runtime_error(buffer);
    }
}

} // end namespace acl
} // end namespace fipa
