#include "message_generator.h"
#include "format/bitefficient_format.h"

namespace fipa {
namespace acl {

MessageGenerator::MessageGenerator() 
{
    mFormats[message_format::BITEFFICIENT] = BitefficientFormat();
}

std::string MessageGenerator::create(const ACLMessage& msg, message_format::Type type) const
{
    std::map<message_format::Type, MessageFormat>::const_iterator it = mFormats.find(type);

    if( it != mFormats.end())
    {
        return it->second.apply(msg);
    } else
    {
        throw std::runtime_error();
    }
}

} // end namespace acl
} // end namespace fipa
