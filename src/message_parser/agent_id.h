#ifndef __FIPAACL_MESSAGEPARSER_AGENTID_H_
#define __FIPAACL_MESSAGEPARSER_AGENTID_H_

#include <vector>
#include <string>

#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/apply_visitor.hpp>

#include <fipa_acl/message_parser/byte_sequence.h>

namespace fipa { namespace acl {

// AgentIdentifier needs Resolvers, since AgentIdentifier is recursive
struct AgentIdentifier;
typedef boost::recursive_wrapper<AgentIdentifier> Resolver;

// To avoid dealing with circular dependecies we use a 'similar' definition to ParameterValue 
typedef boost::variant<std::string, fipa::acl::Resolver, std::vector<fipa::acl::Resolver>, fipa::acl::ByteSequence, fipa::acl::ByteString > UserDefinedParameterValue;

typedef struct
{
	std::string name;
	fipa::acl::UserDefinedParameterValue data;
} UserDefinedParameter;

/**
* \brief Definition for AgentIdentifier
*/
struct AgentIdentifier
{
	std::string name;
	std::vector<std::string> addresses;	
	std::vector<fipa::acl::Resolver> resolvers;
	std::vector<fipa::acl::UserDefinedParameter> parameters;
};

/**
* \brief This class can be used to print the AgentIdentifier
*/
class AgentIdentifierPrinter
{

public:
	AgentIdentifierPrinter();
	~AgentIdentifierPrinter();

        /**
	 * \brief Print an AgentIdentifier
         * \param aid AgentIdentifier
         */
	void print(const fipa::acl::AgentIdentifier& aid);

};

}} // namespace fipa::acl
#endif

