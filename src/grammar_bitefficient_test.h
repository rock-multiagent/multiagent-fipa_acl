#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>

namespace fipa
{
	namespace fusion = boost::fusion;
   	namespace phoenix = boost::phoenix;
    	namespace qi = boost::spirit::qi;
    	namespace ascii = boost::spirit::ascii;


//#############################################################
// The FIPA Bit Message Representation in Bit-Efficient Encoding
//#############################################################

namespace acl
{

struct AgentID;
typedef boost::recursive_wrapper<AgentID> Resolver;

struct AgentID
{
	std::string name;
	std::vector<fipa::acl::Resolver> resolvers;
};

typedef boost::variant< std::string, std::vector<fipa::acl::AgentID> > ParameterValue;

// Define the final message structure here
struct Message
{
	std::string name;
	fipa::acl::ParameterValue parameter;
};


class MessagePrinter 
{
	public:
	MessagePrinter()
	{
	}

	void print(const fipa::acl::Message& msg)
	{	
		printf("Message read.");
	}
};


}

} // end namespace fipa

// The final message structure
BOOST_FUSION_ADAPT_STRUCT(
	fipa::acl::Message,
	(std::string, name)
	(fipa::acl::ParameterValue, data)
)

namespace fipa
{

namespace acl
{

template <typename Iterator>
struct bitefficient_grammar : qi::grammar<Iterator, fipa::acl::Message(), ascii::space_type>
{

	bitefficient_grammar() : bitefficient_grammar::base_type(aclCommunicativeAct, "bitefficient-grammar")
	{
		using qi::byte_;
		using qi::short_;
		using ascii::char_;
		namespace label = qi::labels;
		using phoenix::at_c;
		
	
		aclCommunicativeAct = pName [ at_c<0>(label::_val) = label::_1 ]
				    >> messageParameter [ at_c<1>(label::_val) = label::_1 ];
		messageParameter = *char_ 
				 | pData [ at_c<1>(label::_val) = label::_1 ]
				;
		pData = messageParameter.alias();
	}
	

	qi::rule<Iterator, fipa::acl::Message(),ascii::space_type> aclCommunicativeAct;
	qi::rule<Iterator, fipa::acl::MessageParameter()> messageParameter;
	qi::rule<Iterator, std::string() > pName;
	qi::rule<Iterator> pData;


};

}
} // end namespace fipa


