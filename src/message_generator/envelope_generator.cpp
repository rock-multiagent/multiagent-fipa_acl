#include "envelope_generator.h"
#include <stdexcept>
#include <boost/assign/list_of.hpp>
#include <base/logging.h>

#include "format/bitefficient_envelope_format.h"

namespace fipa {
namespace acl {

std::map<representation::Type, EnvelopeFormatPtr > MessageGenerator::msFormats = boost::assign::map_list_of
    (representation::BITEFFICIENT, boost::shared_ptr<EnvelopeFormat>(new BitefficientEnvelopeFormat()) );

std::string EnvelopeGenerator::create(const ACLEnvelope& envelope, const representation::Type& type)
{
    std::map<representation::Type, EnvelopeFormatPtr >::const_iterator it = msFormats.find(type);

    if( it != msFormats.end())
    {
        return it->second->apply(envelope);
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
