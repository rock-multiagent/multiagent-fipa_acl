#include "message_generator.h"
#include <stdexcept>
#include <boost/assign/list_of.hpp>
#include <base/logging.h>

#include "format/bitefficient_format.h"

namespace fipa {
namespace acl {

std::map<representation::Type, MessageFormatPtr > MessageGenerator::msFormats = boost::assign::map_list_of
    (representation::BITEFFICIENT, boost::shared_ptr<MessageFormat>(new BitefficientFormat()) );

std::string MessageGenerator::create(const ACLMessage& msg, const representation::Type& type)
{
    std::map<representation::Type, MessageFormatPtr >::const_iterator it = msFormats.find(type);

    if( it != msFormats.end())
    {
        return it->second->apply(msg);
    } else
    {
        char buffer[512];
        snprintf(buffer, 512, "Message format of type '%s' is unknown", representation::TypeTxt[type].c_str());
        LOG_ERROR("%s", buffer);
        throw std::runtime_error(buffer);
    }
}

} // end namespace acl
} // end namespace fipa
