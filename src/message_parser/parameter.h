#ifndef __FIPAACL_MESSAGEPARSER_PARAMETER_H_
#define __FIPAACL_MESSAGEPARSER_PARAMETER_H_

#include <fipa_acl/message_parser/date_time.h>
#include <fipa_acl/message_parser/byte_sequence.h>
#include <fipa_acl/message_parser/agent_id.h>

namespace fipa {
namespace acl {
namespace message {

typedef boost::variant<std::string, fipa::acl::AgentIdentifier, std::vector<fipa::acl::AgentIdentifier>, fipa::acl::ByteSequence, fipa::acl::DateTime, fipa::acl::ByteString > ParameterValue;

class ParameterPrinter : public boost::static_visitor<std::string>
{
        public:
                std::string operator()(const std::string& s) const
                {
                        return s;
                }

                std::string operator()(const fipa::acl::AgentIdentifier& agentID) const
                {
                        return "AgentIdentifier";
                }

                std::string operator()(const std::vector<fipa::acl::AgentIdentifier>& ids) const
                {

                        return "AgentIdentifiers";
                }

                std::string operator()(const fipa::acl::ByteSequence& seq) const
                {
                        return "ByteSeq";
                }

                std::string operator()(const fipa::acl::DateTime& dt) const
                {
                        return dt.toString();
                }

                std::string operator()(const fipa::acl::ByteString& bs) const
                {
                        return "ByteString";
                }

};

} // end namespace message

namespace envelope {

typedef std::string ParameterValue;

} // end namespace envelope

} // end namespace acl
} // end namespace fipa
#endif
