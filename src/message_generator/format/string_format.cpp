#include "string_format.h"

namespace fipa {
namespace acl {

std::string StringFormat::getAgentIdentifier(const AgentID& agent)
{
    std::string agentTxt = "(agent-identifier";
    agentTxt += ":name" + getWord(agent.getName());
    if(!agent.getAddresses().empty())
    {
        agentTxt += ":addresses" + getUrlSequence(agent.getAddresses());
    }
    if(!agent.getResolvers().empty())
    {
        agentTxt += ":resolvers" + getAgentIdentifierSequence(agent.getResolvers());
    }

    std::vector<UserdefParam>::const_iterator cit = agent.getUserdefParams().begin();
    for(; cit != agent.getUserdefParams().end(); ++cit)
    {
        agentTxt += getUserdefinedParameter(cit->getName());
        agentTxt += getExpression(cit->getValue());
    }
    agentTxt += ")";

    return agentTxt;
}

std::string StringFormat::getAgentIdentifierSequence(const AgentIDList& agentList)
{
    std::string list = "(sequence";
    list += getAgentIdentifierList(agentList);
    list += ")";
    return list;
}

std::string StringFormat::getAgentIdentifierSet(const AgentIDList& agentList)
{
    std::string list = "(set";
    list += getAgentIdentifierList(agentList);
    list += ")";
    return list;
}

std::string StringFormat::getAgentIdentifierList(const AgentIDList& agentList)
{
    std::string list = "";
    AgentIDList::const_iterator cit = agentList.begin();
    for(; cit != agentList.end(); ++cit)
    {
        list += getAgentIdentifier(*cit);
    }
    return list;
}

std::string StringFormat::getUrlSequence(const Addresses& addresses)
{
    std::string sequence = "(sequence";
    Addresses::const_iterator cit = addresses.begin();
    for(; cit != addresses.end(); ++cit)
    {
        sequence += getUrl(*cit);
    }
    sequence += ")";
    return sequence;
}


} // end namespace acl
} // end namespace fipa
