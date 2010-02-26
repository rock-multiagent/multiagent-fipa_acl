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

typedef struct
{
	std::string name;
	std::string data;
} MessageParameter;


// Define the final message structure here
struct Message
{
	std::string word;
	fipa::acl::MessageParameter parameter;
};

class MessagePrinter 
{
	public:
	MessagePrinter()
	{
	}

	void print(const fipa::acl::Message& msg)
	{	
		printf("Message read:");
		printf("word:          %s\n", msg.word.c_str());
	}
};


}

} // end namespace fipa

BOOST_FUSION_ADAPT_STRUCT(
	fipa::acl::MessageParameter,
	(std::string, name)
	(std::string, data)
)
// The final message structure
BOOST_FUSION_ADAPT_STRUCT(
	fipa::acl::Message,
	(std::string, word)
	(fipa::acl::MessageParameter, parameter)
)

namespace fipa
{

	// In order to use functions as semantic actions
	// lazy evaluation is required	
	struct extractFromCodetableImpl
	{
		template <typename T>
		struct result
		{
			typedef std::string type;
		};

		template <typename T>
		std::string operator()(T arg) const
		{
			unsigned short index;
			if(typeid(T) == typeid(unsigned short))
			{
				std::cout << "Unsigned short" << std::endl;
				index = arg;
			}


			switch(index)
			{
				case 1:
					return std::string("ONE");
					break;
				case 2:
					return std::string("TWO");
					break;
			}

			return std::string("UNKNOWN");
		}
	
	};

	phoenix::function<extractFromCodetableImpl> extractFromCodetable;


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
		
	
		aclCommunicativeAct = messageParameter [ at_c<1>(label::_val) = label::_1 ];
		messageParameter =  pName [ at_c<0>(label::_val) = label::_1 ]
				 >> pData [ at_c<1>(label::_val) = label::_1 ]

				;
		pName = *char_;
		pData = *char_;
	
	}
	

	qi::rule<Iterator, fipa::acl::Message(),ascii::space_type> aclCommunicativeAct;
	qi::rule<Iterator, fipa::acl::MessageParameter()> messageParameter;
	qi::rule<Iterator, std::string() > pName;
	qi::rule<Iterator, std::string() > pData;


};

}
} // end namespace fipa


