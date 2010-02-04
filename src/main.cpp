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
		using qi::long_;
		using ascii::digit;
		using ascii::alpha;

		namespace label = qi::labels;
		using phoenix::at_c;
		
	
		//lexeme[x] disables skip parsing for x	
		aclCommunicativeAct = message;
		message = header >> messageType >> *messageParameter >> endOfMessage;
		header %= (messageId >> version); 			// [ std::cout << "Header" << label::_1 << std::endl] ;
		messageId %= ( byte_(0xFA) | byte_(0xFB) | byte_(0xFC) ) [ std::cout << "MessageId: " << label::_1 << std::endl ];
		version %= byte_ 					 [ std::cout << "version: " << label::_1 << std::endl ];
		endOfMessage %= endOfCollection;
		endOfCollection %= byte_(0x01)   			 [ std::cout << "endOfCollection: " <<  label::_1 << std::endl ];
		messageType %= predefinedMessageType | userDefinedMessageType;
		userDefinedMessageType %= byte_(0x00) >> messageTypeName;
		messageTypeName %= binWord;
	
		messageParameter %= predefinedMessageParameter | userDefinedMessageParameter;
		userDefinedMessageParameter %= byte_(0x00) >> parameterName >> parameterValue;
		parameterName %= binWord;

		parameterValue %= binExpression;
		predefinedMessageType %= byte_(0x01) 	// accept-proposal 	
					| byte_(0x02)   // agree 		
					| byte_(0x03)   // cancel 	
					| byte_(0x04)   // cfp    
					| byte_(0x05)   // confirm
					| byte_(0x06)   // disconfirm 	
					| byte_(0x07)   // failure
					| byte_(0x08)   // inform
					| byte_(0x09)   // inform-if
					| byte_(0x0a)   // inform-ref
					| byte_(0x0b)   // not-understood
					| byte_(0x0c)   // propagate 	
					| byte_(0x0d)   // propose 
					| byte_(0x0e)   // proxy 
					| byte_(0x0f)   // query-if
					| byte_(0x10)   // query-ref
					| byte_(0x11)   // refuse   
					| byte_(0x12)   // reject-proposal
					| byte_(0x13)   // request       
					| byte_(0x14)   // request-when 
					| byte_(0x15)   // request-whenever
					| byte_(0x16);  // subscribe      

		predefinedMessageParameter %= byte_(0x02) >> agentIdentifier  // sender
					| byte_(0x03) >>  recipientExpr      // receiver 
					| byte_(0x04) >> msgContent          // content 
					| byte_(0x05) >> replyWithParam      // reply-with
					| byte_(0x06) >> replyByParam        // reply-by 
					| byte_(0x07) >> inReplyToParam      // in-reply-to 
					| byte_(0x08) >> replyToParam        // reply-to   
					| byte_(0x09) >> language            // language  
					| byte_(0x0a) >> encoding            // encoding 
					| byte_(0x0b) >> ontology            // ontology
					| byte_(0x0c) >> protocol            // protocol
					| byte_(0x0d) >> conversationId;     // conversation-id
					 
		agentIdentifier %= byte_(0x02) >> agentName >> -addresses >> -resolvers >> *(userDefinedParameter) >> endOfCollection;				
		agentName %= binWord;
		addresses %= byte_(0x02) >> urlCollection;
		resolvers %= byte_(0x03) >> agentIdentifierCollection;
		userDefinedParameter %= byte_(0x04) >> binWord >> binExpression;
		
		urlCollection %= *url >> endOfCollection;
		url %= binWord;
		agentIdentifierCollection %= *(agentIdentifier) >> endOfCollection;
	 	recipientExpr %= agentIdentifierCollection;
		msgContent %= binString;	

		replyWithParam %= binExpression;
		replyByParam %= binDateTimeToken;
		inReplyToParam %= binExpression;
		replyToParam %= recipientExpr;
		language %= binExpression;
		encoding %= binExpression;
		ontology %= binExpression;
		protocol %= binWord;
		conversationId %= binExpression;
	

		binWord %= ( ( byte_(0x10) >> word >> byte_(0x00) ) | byte_(0x11) >> index);
		binNumber %= ( byte_(0x12) >> digits )       // Decimal numbers
			  | ( byte_(0x13) >> digits );  	// Hexadecimal numbers

		digits %= +codedNumber;

		binString %= ( byte_(0x14) >> fipaString >> byte_(0x00) ) // new string literal 
			  | ( byte_(0x15) >> index )                     // string literal from code table
			  | ( byte_(0x16) >> len8 >> byteSeq )           // new byteLengthEncoded string 
			  | ( byte_(0x17) >> len16 >> byteSeq )          // new byteLengthEncoded string
			  | ( byte_(0x18) >> index )                     // byteLengthEncoded from code table
			  | ( byte_(0x19) >> len32 >> byteSeq );         // new byteLengthEncoded string   

		binDateTimeToken %= ( byte_(0x20) >> binDate )		            // Absolute time  
				| ( byte_(0x21) >> binDate )		            // Relative time (+)
				| ( byte_(0x22) >> binDate )		            // Relative time (-)
				| ( byte_(0x24) >> binDate >> typeDesignator )      // Absolute time   
				| ( byte_(0x25) >> binDate >> typeDesignator )      // Relative time (+)
				| ( byte_(0x26) >> binDate >> typeDesignator );     // Relative time (-)

		binDate %= year >> month >> day >> hour >> minute >> second >> millisecond;
		
		binExpression %= binExpr | byte_(0xFF) >> binString;
		binExpr %= binWord
			| binString
			| binNumber
			| (exprStart >> *binExpr >> exprEnd);

		exprStart %= ( byte_(0x60) )
			  | ( byte_(0x70) >> word >> byte_(0x00) )
			  | ( byte_(0x71) >> index )
			  | ( byte_(0x72) >> digits )
			  | ( byte_(0x73) >> digits )
			  | ( byte_(0x74) >> fipaString >> byte_(0x00) )
			  | ( byte_(0x75) >> index )
			  | ( byte_(0x76) >> len8 >> fipaString )
			  | ( byte_(0x77) >> len16 >> fipaString )
			  | ( byte_(0x78) >> len32 >> fipaString )
			  | ( byte_(0x79) >> index );

		exprEnd %= ( byte_(0x40) )
			| ( byte_(0x50) >> word >> byte_(0x00) )
			| ( byte_(0x51) >> index )
			| ( byte_(0x52) >> digits )
			| ( byte_(0x53) >> digits )
			| ( byte_(0x54) >> fipaString >> byte_(0x00) )
			| ( byte_(0x55) >> index ) 
			| ( byte_(0x56) >> len8 >> fipaString )
			| ( byte_(0x57) >> len16 >> fipaString )
			| ( byte_(0x58) >> len32 >> fipaString )
			| ( byte_(0x59) >> index );

		index %= byte_ | short_;
		byteSeq %= *byte_;
		
		index %= ( byte_ | short_ );
		len8 %= byte_;
		len16 %= short_;
		len32 %= long_;
	
		year %= byte_ >> byte_;
		month %= byte_;
		day %= byte_;
		hour %= byte_;
		minute %= byte_;
		second %= byte_;
		millisecond %= byte_ >> byte_;

		word %= *char_;  //TODO: character exceptions
		fipaString %= stringLiteral | byteLengthEncodedString;
		stringLiteral %= char_('"');// >> TODO: ([ ~ "\"" ] | "\\\"")*  char_('"');
		byteLengthEncodedString %= char_('#');// >> +digit >> char_("\"") >> *byte_;

		codedNumber %= byte_; // two number in one byte - use padding 00 if coding only one number
		typeDesignator %= alpha;
	}

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
	qi::rule<Iterator> messageParameter;
	qi::rule<Iterator> userDefinedMessageParameter;
	
	qi::rule<Iterator> parameterName;
	qi::rule<Iterator> parameterValue;

	qi::rule<Iterator> predefinedMessageType;
	qi::rule<Iterator> predefinedMessageParameter;
	
	qi::rule<Iterator> agentIdentifier;
	qi::rule<Iterator> agentName;
	qi::rule<Iterator> addresses;
	qi::rule<Iterator> resolvers;
 	qi::rule<Iterator> userDefinedParameter;
	
	qi::rule<Iterator> urlCollection;
	qi::rule<Iterator> url;
	qi::rule<Iterator> agentIdentifierCollection;
	qi::rule<Iterator> recipientExpr;
	qi::rule<Iterator> msgContent;
	qi::rule<Iterator> replyWithParam;
	qi::rule<Iterator> replyByParam;
	qi::rule<Iterator> inReplyToParam;
	qi::rule<Iterator> replyToParam;
	qi::rule<Iterator> language;
	qi::rule<Iterator> encoding;
	qi::rule<Iterator> ontology;
	qi::rule<Iterator> protocol;
	qi::rule<Iterator> conversationId;
	
	qi::rule<Iterator> binWord;
	qi::rule<Iterator> binNumber;
	qi::rule<Iterator> digits; 
	qi::rule<Iterator> binString;
	qi::rule<Iterator> binDateTimeToken;
	qi::rule<Iterator> binDate;
	qi::rule<Iterator> binExpression;
	qi::rule<Iterator> binExpr;
	qi::rule<Iterator> exprStart;
	qi::rule<Iterator> exprEnd;

	qi::rule<Iterator> byteSeq;
	qi::rule<Iterator> index;
	qi::rule<Iterator> len8;
	qi::rule<Iterator> len16;
	qi::rule<Iterator> len32;
	qi::rule<Iterator> year;
	qi::rule<Iterator> month;
	qi::rule<Iterator> day;
	qi::rule<Iterator> hour;
	qi::rule<Iterator> minute;
	qi::rule<Iterator> second;
	qi::rule<Iterator> millisecond;

	qi::rule<Iterator> word;
	qi::rule<Iterator> fipaString;
	qi::rule<Iterator> stringLiteral;
	qi::rule<Iterator> byteLengthEncodedString;
	qi::rule<Iterator> codedNumber;
	qi::rule<Iterator> typeDesignator;

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

