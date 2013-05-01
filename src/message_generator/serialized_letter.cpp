#include "serialized_letter.h"
#include <fipa_acl/message_parser/envelope_parser.h>
#include <fipa_acl/message_generator/envelope_generator.h>
#include <stdexcept>

namespace fipa {

SerializedLetter::SerializedLetter()
    : representation(fipa::acl::representation::UNKNOWN)
    , timestamp(base::Time::now())
{
}


SerializedLetter::SerializedLetter(const fipa::acl::Letter& letter, fipa::acl::representation::Type _representation)
    : representation(_representation)
    , timestamp(base::Time::now())
{
    std::string serializedLetter = fipa::acl::EnvelopeGenerator::create(letter, representation);
    setData(serializedLetter);
}


fipa::acl::Letter SerializedLetter::getLetter() const
{
    fipa::acl::EnvelopeParser ep;
    fipa::acl::Letter letter;

    std::string serializedLetter = getDataAsString();
    if( ep.parseData(serializedLetter, letter, representation) )
    {
        return letter;
    }

    throw std::runtime_error("SerializedLetter: could not parse data correctly");
}

void SerializedLetter::setData(const std::string& msg)
{
    for(unsigned int i=0; i<msg.size(); i++)
    {
        this->data.push_back(msg[i]);
    } 
}

std::string SerializedLetter::getDataAsString() const 
{
    std::string str = "";
    for (std::vector<uint8_t>::const_iterator it = data.begin(); it != data.end(); it++)
	    str += (*it);
    return str;
}

} // end namespace fipa
