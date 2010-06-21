#ifndef __FIPAACL_MESSAGEPARSER_PARAMETER_H_
#define __FIPAACL_MESSAGEPARSER_PARAMETER_H_

#include "date_time.h"
#include "byte_sequence.h"
#include "agent_id.h"

namespace fipa { namespace acl {

typedef boost::variant<std::string, fipa::acl::AgentID, std::vector<fipa::acl::AgentID>, fipa::acl::ByteSequence, fipa::acl::DateTime, fipa::acl::ByteString > ParameterValue;

class ParameterPrinter : public boost::static_visitor<std::string>
{
        public:
                std::string operator()(std::string s) const
                {
                        return s;
                }

                std::string operator()(fipa::acl::AgentID agentID) const
                {
                        return "AgentID";
                }

                std::string operator()(std::vector<fipa::acl::AgentID> ids) const
                {

                        return "AgentIDs";
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
