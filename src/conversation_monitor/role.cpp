#include "role.h"
#include <stdexcept>
#include <cassert>
#include <boost/regex.hpp>
#include <base/logging.h>

namespace fipa {
namespace acl {

const RoleId Role::UNDEFINED_ID = "__undefined__";
const RoleId Role::SELF_ID ="__self__";

Role::Role()
    : mId(Role::UNDEFINED_ID)
{
}

Role::Role(const RoleId& id)
    : mId(id)
{
}

UndefinedRole::UndefinedRole()
    : Role(Role::UNDEFINED_ID)
{}

SelfRole::SelfRole()
    : Role(Role::SELF_ID)
{}

RoleMapping::RoleMapping()
{
    addRole(SelfRole());
    addExpectedAgent(SelfRole(), UndefinedAgentID());
}

void RoleMapping::setSelf(const AgentID& id)
{
    std::map<Role, AgentIDList>::iterator it = mExpectedAgentMapping.find(SelfRole());
    // Internal error if AgentID for 'self' is not set
    assert(!it->second.empty());

    if(it->second.front() == UndefinedAgentID())
    {
        it->second.clear();
        addExpectedAgent(SelfRole(), id);
    } else {
        std::string msg = "Self is already set";
        LOG_WARN_S << msg;
        throw std::runtime_error(msg);
    }
}

AgentID RoleMapping::getSelf() const
{
    std::map<Role, AgentIDList>::const_iterator it = mExpectedAgentMapping.find(SelfRole());
    if(it != mExpectedAgentMapping.end())
    {
        return it->second.front();
    } else {
        // AgentID has never been set -- should never happen 
        assert(false);
    }

    return UndefinedAgentID();
}

bool RoleMapping::isSelf(const AgentID& id) const
{
    return getSelf() == id;
}

void RoleMapping::addRole(const Role& role)
{
    mExpectedAgentMapping.insert( std::pair<Role, AgentIDList>(role, AgentIDList()));
}

void RoleMapping::addExpectedAgent(const Role& role, const AgentID& agent)
{
    std::map<Role, AgentIDList>::iterator it = mExpectedAgentMapping.find(role);
    if(it == mExpectedAgentMapping.end())
    {
        std::string msg = "Unexpected role '" + role.toString() + "'.";
        LOG_ERROR("%s", msg.c_str());
        throw std::runtime_error(msg);
    }
   
    AgentIDList& expectedAgents = it->second; 
    AgentIDList::iterator eit = std::find(expectedAgents.begin(), expectedAgents.end(), agent);
    if(eit == expectedAgents.end())
    {
        expectedAgents.push_back(agent);
    }

    return;
}

bool RoleMapping::isExpected(const Role& role, const AgentID& agent) const
{
    std::map<Role, AgentIDList>::const_iterator it = mExpectedAgentMapping.find(role);
    if(it == mExpectedAgentMapping.end())
    {
        std::string msg = "Unexpected role '" + role.toString() + "'.";
        LOG_ERROR("%s", msg.c_str());
        throw std::runtime_error(msg);
    }

    bool expected = false;
    const AgentIDList& expectedAgents = it->second;
    AgentIDList::const_iterator eit = expectedAgents.begin(); 
    for(; eit != expectedAgents.end(); ++eit)
    {
        boost::regex regex(eit->getName());
        if(regex_match(agent.getName(), regex))
        {
            expected = true;
        }
    }

    return expected;
}

} 
} // end namespace fipa::acl
