#include <iostream>

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

#include <iostream>
#include <fstream>
#include <string>
#include <vector>


namespace fipa
{
	namespace fusion = boost::fusion;
   	namespace phoenix = boost::phoenix;
    	namespace qi = boost::spirit::qi;
    	namespace ascii = boost::spirit::ascii;


//#############################################################
// The FIPA Bit Message Representation in Bit-Efficient Encoding
//#############################################################

struct ACLMessage
{
	char id;
	char version;
};

class MessagePrinter 
{
	public:
	MessagePrinter()
	{
	}

	void print(const ACLMessage& msg)
	{
		printf("Id: %x", msg.id);// version %x",msg.id,msg.version);
	}

};

} // end namespace fipa


// We need to tell fusion about the message structur to make
// it a first class fusion citizen
BOOST_FUSION_ADAPT_STRUCT(
	fipa::ACLMessage,
	(char, id)
	(char, version)
)

namespace fipa
{

	struct print_action
	{
		void operator()(char const& c, qi::unused_type, qi::unused_type) const
		{
			printf("Hex: %x", c);
		}
	};

template <typename Iterator>
struct bitefficient_grammar : qi::grammar<Iterator, ACLMessage, ascii::space_type>
{

	bitefficient_grammar() : bitefficient_grammar::base_type(aclCommunicativeAct, "bitefficient-grammar")
	{
		using qi::lit;
		using qi::lexeme;
		using ascii::char_;
		using ascii::string;
		using qi::byte_;
		using qi::short_;
		namespace label = qi::labels;
		using phoenix::at_c;
		
	
		//lexeme[x] disables skip parsing for x	
		aclCommunicativeAct = message;
		message = header >> messageType >> endOfMessage;
		header %= (messageId >> version); 			// [ std::cout << "Header" << label::_1 << std::endl] ;
		messageId %= ( byte_(0xFA) | byte_(0xFB) | byte_(0xFC) ) [ std::cout << "MessageId: " << label::_1 << std::endl ];
		version %= byte_ 					 [ std::cout << "version: " << label::_1 << std::endl ];
		endOfMessage %= endOfCollection;
		endOfCollection %= byte_(0x01)   			 [ std::cout << "endOfCollection: " <<  label::_1 << std::endl ];
		messageType = /*predefinedMessageType |*/ userDefinedMessageType;
		userDefinedMessageType = byte_(0x00) >> messageTypeName;
		messageTypeName = binWord;
		binWord = ( ( byte_(0x10) >> word >> byte_(0x00) ) | byte_(0x11) >> index);
		word = *char_ /* exceptions*/ ;
		index = byte_ | short_;
	}

	//qi::rule<Iterator, char, ascii::space_type> id;
	qi::rule<Iterator, ACLMessage, ascii::space_type> aclCommunicativeAct;
	qi::rule<Iterator> message;
	qi::rule<Iterator> header;
	qi::rule<Iterator> messageId;
	qi::rule<Iterator> version;
	qi::rule<Iterator> endOfMessage;
	qi::rule<Iterator> endOfCollection;

	qi::rule<Iterator> messageType;
	qi::rule<Iterator> userDefinedMessageType;
	qi::rule<Iterator> messageTypeName;
	qi::rule<Iterator> binWord;
	qi::rule<Iterator> word;
	qi::rule<Iterator> index;

};


} // end namespace fipa

////////////////////////////////////////////////////////////////////////////
//  Main program
////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
	char const* filename;
	if(argc > 1)
	{
		filename = argv[1];
	} else
	{
		std::cerr << "Error: no input file provided." << std::endl;
		return 1;
	}

	std::ifstream in(filename, std::ios_base::in);

	if (!in)
	{
		std::cerr << "Error: could not open input file: "
			<< filename << std::endl;
		return 1;
	}

	std::string storage; 
//	in.unsetf(std::ios::skipws); // No white space skipping
	std::copy(
		std::istream_iterator<char>(in),
		std::istream_iterator<char>(),
		std::back_inserter(storage));

	typedef fipa::bitefficient_grammar<std::string::const_iterator> bitefficient_grammar;
	bitefficient_grammar grammar;
	fipa::ACLMessage parseTree;

	using boost::spirit::ascii::space;
	std::string::const_iterator iter = storage.begin();
	std::string::const_iterator end = storage.end();
	bool r = phrase_parse(iter, end, grammar, space, parseTree);

	if(r && iter == end)
	{
		std::cout << "------------------------------------\n";
		std::cout << " Parsing succeeded\n";
		std::cout << "------------------------------------\n";
		fipa::MessagePrinter printer;
		printer.print(parseTree);

	        return 0;	
	} else {

		std::cout << " Parsing failed\n";
	}

    return 0;
}

