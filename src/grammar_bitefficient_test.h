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


// Embedd only types into the BOOST_FUSION_ADAPT_STRUCT
// So perform all definitions here
//
typedef struct 
{
	char id;
	char version;
} Header;

// Define the final message structure here
struct Message
{
	Header header;
};

}

} // end namespace fipa


// We need to tell fusion about the message structure to make
// This includes all composite types
BOOST_FUSION_ADAPT_STRUCT(
	fipa::acl::Header,
	(char, id)
	(char, version)
)

// The final message structure
BOOST_FUSION_ADAPT_STRUCT(
	fipa::acl::Message,
	(fipa::acl::Header, header)
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
		namespace label = qi::labels;
		using phoenix::at_c;
		
	
		aclCommunicativeAct = header [ at_c<0>(label::_val) = label::_1 ];
		header = (byte_ [at_c<0>(label::_val) = label::_1] >> byte_ [ at_c<1>(label::_val) = label::_1] ); 
	}
	
	qi::rule<Iterator, fipa::acl::Message(), ascii::space_type> aclCommunicativeAct;
	qi::rule<Iterator, fipa::acl::Header()> header;
};

}
} // end namespace fipa


