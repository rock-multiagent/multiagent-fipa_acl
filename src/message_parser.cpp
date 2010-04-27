#include "message_parser.h"
#include "grammar_bitefficient.h"

namespace fipa { namespace acl {


MessageParser::MessageParser()
{
}

MessageParser::~MessageParser()
{
}

fipa::acl::ACLMessage parseData(const std::vector<uint8_t>& data)
{
 
  fipa::acl::ACLMessage message;

  // use the stuff as shown in the main.cpp an use the tools given in bitefficient_grammar
  // to populate the ACLMessage

}

}}
