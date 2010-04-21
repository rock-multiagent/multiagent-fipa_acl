#include "message_parser.h"
#include "bitefficient_grammar.h"

namespace fipa { acl {


MessageParser::MessageParser()
{
}

MessageParser::~MessagerParser()
{
}

fipa::acl::ACLMessage parseData(const std::vector<uint_8_t>& data)
{
 
  fipa::acl::ACLMessage message;

  // use the stuff as shown in the main.cpp an use the tools given in bitefficient_grammar
  // to populate the ACLMessage

}

}}
