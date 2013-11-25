#ifndef FIPA_ACL_STRING_FORMAT_H
#define FIPA_ACL_STRING_FORMAT_H

#include <fipa_acl/message_generator/message_format.h>
namespace fipa {
namespace acl {

class StringFormat
{
public:
    static std::string getAgentIdentifier(const AgentID& agentID);

    /**
     * Get encoded agent identifier list (with sequence label)
     */
    static std::string getAgentIdentifierSequence(const AgentIDList& agentList);

    /**
     * Get encoded agent identifier list (with set label)
     */
    static std::string getAgentIdentifierSet(const AgentIDList& agentList);

    /**
     * Get encoded agent identifier list
     */
    static std::string getAgentIdentifierList(const AgentIDList& agentList);

    /**
     * Get Expression
     * Expression is wrapped using parentheses to facilitate parsing, i.e. to account for too greedy parser behaviour,
     * especially when using userdefined parameters
     *
     */
    static std::string getExpression(const std::string& txt) { return "(" + txt + ")"; }

    static std::string getWord(const std::string& txt) { return txt; }

    static std::string getUrlSequence(const Addresses& addresses);

    static std::string getUserdefinedParameter(const std::string& param) { return param; }

    static std::string getUrl(const std::string& url) { return url; }
};

} // end namespace acl
} // end namespace fipa

#endif //FIPA_ACL_STRING_FORMAT_H
