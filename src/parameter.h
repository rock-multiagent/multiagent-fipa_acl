#ifndef __FIPAACL_MESSAGEPARSER_PARAMETER_H_
#define __FIPAACL_MESSAGEPARSER_PARAMETER_H_

#include <message-parser/date_time.h>
#include <message-parser/byte_sequence.h>
#include <message-parser/agent_id.h>

namespace fipa { namespace acl {

typedef boost::variant<std::string, fipa::acl::AgentIdentifier, std::vector<fipa::acl::AgentIdentifier>, fipa::acl::ByteSequence, fipa::acl::DateTime, fipa::acl::ByteString > ParameterValue;

class ParameterPrinter : public boost::static_visitor<std::string>
{
        public:
                std::string operator()(std::string s) const
                {
                        return s;
                }

                std::string operator()(fipa::acl::AgentIdentifier agentID) const
                {
                        return "AgentIdentifier";
                }

                std::string operator()(std::vector<fipa::acl::AgentIdentifier> ids) const
                {

                        return "AgentIdentifiers";
                }

                std::string operator()(fipa::acl::ByteSequence seq) const
                {
                        return "ByteSeq";
                }

                std::string operator()(fipa::acl::DateTime dt) const
                {
                        return dt.toString();
                }

                std::string operator()(fipa::acl::ByteString bs) const
                {
                        return "ByteString";
                }

};

}}

#endif
