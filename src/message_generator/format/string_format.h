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
     * Byte-length encoded string format.
     */
    static std::string getString(const std::string& txt);

    /**
     * Get Expression
     * Expression is encoded as a byte-length encoded string.
     */
    static std::string getExpression(const std::string& txt) { return getString(txt); }

    static std::string getWord(const std::string& txt) { return txt; }    

    static std::string getUrlSequence(const Addresses& addresses);

    static std::string getUserdefinedParameter(const std::string& param) { return param; }

    static std::string getUrl(const std::string& url) { return url; }
};

} // end namespace acl
} // end namespace fipa

#endif //FIPA_ACL_STRING_FORMAT_H
