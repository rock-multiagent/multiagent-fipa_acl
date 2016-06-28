#include "envelope_generator.h"
#include <stdexcept>
#include <boost/assign/list_of.hpp>
#include <base-logging/Logging.hpp>

#include "format/bitefficient_envelope_format.h"
#include "format/xml_envelope_format.h"

namespace fipa {
namespace acl {

std::map<representation::Type, EnvelopeFormatPtr > EnvelopeGenerator::msFormats = boost::assign::map_list_of
    (representation::BITEFFICIENT, boost::shared_ptr<EnvelopeFormat>(new BitefficientEnvelopeFormat()) )
    (representation::XML, boost::shared_ptr<EnvelopeFormat>(new XMLEnvelopeFormat()) );

std::string EnvelopeGenerator::create(const ACLEnvelope& envelope, const representation::Type& type)
{
    std::map<representation::Type, EnvelopeFormatPtr >::const_iterator it = msFormats.find(type);

    if( it != msFormats.end())
    {
        return it->second->apply(envelope);
    } else
    {
        char buffer[512];
        snprintf(buffer, 512, "fipa::acl::EnvelopeGenerator: envelope format of type '%s' is unknown", representation::TypeTxt[type].c_str());
        LOG_ERROR("%s", buffer);
        throw std::runtime_error(buffer);
    }
}


} // end namespace acl
} // end namespace fipa
