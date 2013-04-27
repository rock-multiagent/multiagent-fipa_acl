#include "envelope_parser.h"
#include "grammar/grammar_bitefficient_envelope.h"

namespace fipa {
namespace acl {

bool EnvelopeParser::parseData(const std::string& storage, ACLEnvelope& envelope, representation::Type type)
{
    typedef fipa::acl::bitefficient::Envelope<std::string::const_iterator> bitefficient_envelope_grammar;
    bitefficient_envelope_grammar grammar;

    fipa::acl::ACLEnvelope parseTree;
    std::string::const_iterator iter = storage.begin(); 
    std::string::const_iterator end = storage.end(); 
    bool r = parse(iter, end, grammar, parseTree);

    if(r && iter == end)
    {
        return true;
    }
    return false;
}

} // end namespace acl
} // end namespace fipa
