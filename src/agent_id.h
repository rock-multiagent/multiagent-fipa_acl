#ifndef __FIPAACL_MESSAGEPARSER_AGENTID_H_
#define __FIPAACL_MESSAGEPARSER_AGENTID_H_

#include <vector>
#include <string>

#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/apply_visitor.hpp>

#include "byte_sequence.h"

namespace fipa { namespace acl {

// AgentID needs Resolvers, since AgentID is recursive
struct AgentID;
typedef boost::recursive_wrapper<AgentID> Resolver;

// To avoid dealing with circular dependecies we use a 'similar' definition to ParameterValue 
typedef boost::variant<std::string, fipa::acl::Resolver, std::vector<fipa::acl::Resolver>, fipa::acl::ByteSequence, fipa::acl::ByteString > UserDefinedParameterValue;

typedef struct
{
	std::string name;
	fipa::acl::UserDefinedParameterValue data;
} UserDefinedParameter;


struct AgentID
{
	std::string name;
	std::vector<std::string> addresses;	
	std::vector<fipa::acl::Resolver> resolvers;
	std::vector<fipa::acl::UserDefinedParameter> parameters;
};

class AgentIDPrinter
{

public:
	AgentIDPrinter();
	~AgentIDPrinter();
	
	void print(const fipa::acl::AgentID& aid);

};

}} // namespace fipa::acl
#endif

