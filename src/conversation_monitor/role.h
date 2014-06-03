#ifndef FIPA_ACL_ROLE_H
#define FIPA_ACL_ROLE_H

#include <map>
#include <string>
#include <fipa_acl/message_generator/agent_id.h>

namespace fipa {
namespace acl {

class AgentID;

typedef std::string RoleId;

class Role 
{

private:
    RoleId mId;

protected:
    /**
     * Interal role ids
     */
    const static RoleId UNDEFINED_ID;
    const static RoleId SELF_ID;

public:
    /**
     * Role default constructor (required for map implementation)
     */
    Role();

    /**
     * Role default constructor
     */
    Role(const RoleId& id);

    /**
     * Copy constructor
     */
    Role(const Role&);

    /**
     * Role self
     */
    static const Role self();

    /**
     * Get role id
     */
    RoleId getId() const { return mId; }

    /**
     * Convert role to a string
     * \return string
     */
    std::string toString() const { return mId; }

    bool operator<(const Role& other) const
    {
        return mId < other.mId;
    }

    bool operator!=(const Role& other) const
    {
        return !(mId == other.mId);
    }

    bool operator==(const Role& other) const
    {
        return mId == other.mId;
    }

    Role& operator=(const Role& other)
    {
        mId = other.mId;
        return *this;
    }
};

class UndefinedRole : public Role
{
public:
    UndefinedRole();
};

class SelfRole : public Role
{
public:
    SelfRole();
};

/**
 * \class RoleMapping
 * \brief Mapping between roles and actual agent ids 
 */
class RoleMapping
{
private:
    // The mapping between roles and actual agents
    std::map<Role, AgentIDList> mExpectedAgentMapping;

public:

    /**
     * Default constructor
     */
    RoleMapping();

    /**
     * Returns the mapping
     */
    const std::map<Role, AgentIDList>& getMapping() const { return mExpectedAgentMapping; }

    /**
     * Add the role 'self'
     */
    void setSelf(const AgentID& id);

    /**
     * Test whether an agent id represents 'self'
     */
    bool isSelf(const AgentID& id) const;

    /**
     * Retrieve the AgentID of self
     */
    AgentID getSelf() const;

    /**
     * Add a role to this rolemapping
     */
    void addRole(const Role& role);

    /**
     * Add an agent to the list of expected agents for a specific role
     */
    void addExpectedAgent(const Role& role, const AgentID& agent);

    /**
     * Clear list of expected agents for a specific role
     */
    void clearExpectedAgents(const Role& role);

    /**
     * Clear list of expected agents for all known roles
     *
     * Note that the self role remains with will be reset to UndefinedAgentID
     */
    void resetExpectedAgents();

    /**
     * Check whether the given agent belongs to the list of expected receivers
     * \throws If role is completely unknown to this role mapping
     */
    bool isExpected(const Role& role, const AgentID& agent) const;
};

} // end namespace acl
} // end namespace fipa
#endif // FIPA_ACL_ROLE_H
