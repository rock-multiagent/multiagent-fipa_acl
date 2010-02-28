/**
 *
 * \file grammar_bitefficient.h
 * \author Thomas Roehr, thomas.roehr@dfki.de
 */

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>

#include <boost/spirit/home/support/context.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>

namespace fusion = boost::fusion;
namespace phoenix = boost::phoenix;
namespace spirit = boost::spirit;
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

namespace fipa
{

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

struct AgentID;

typedef boost::recursive_wrapper<AgentID> Resolver;

struct AgentID
{
	std::string name;
	std::vector<std::string> addresses;	
	std::vector<fipa::acl::Resolver> resolvers;
};

typedef boost::variant<std::string,fipa::acl::AgentID> ParameterValue;
typedef struct
{
	std::string name;
	fipa::acl::ParameterValue data;
} MessageParameter;


// Define the final message structure here
struct Message
{
	Message() : type(""), parameters() {}

	Header header;
	std::string type;
	std::vector<fipa::acl::MessageParameter> parameters;
};


class MessagePrinter 
{
	public:
	MessagePrinter()
	{
	}

	void print(const fipa::acl::Message& msg)
	{	
		printf("FIPA Message read:\n");
		printf("id:          %x\n", msg.header.id);
		switch(msg.header.id)
		{
			case 0xfa: printf("\tid is FA\n"); break;
			case 0xfb: printf("\tid is FB\n"); break;
			case 0xfc: printf("\tid is FC\n"); break;
			default: printf("\tWARNING: id is unknown\n");
		}

		printf("version:                  %x\n", msg.header.version); 
		printf("performative(type):       %s\n", msg.type.c_str());
		
		printf("msg-parameters count:     %d\n", msg.parameters.size());
		for(int i=0; i < msg.parameters.size(); i++)
		{
			MessageParameter mp = msg.parameters[i];
			printf("\tparameter #%d:  %s\n",i, mp.name.c_str()); 
		}
	
	}
};

} // end namespace acl


} // end namespace fipa

// ########################################################
// BOOST_FUSION_ADAPTION
// #######################################################
//
// We need to tell boost::fusion about the message structure to make
// This includes all composite types like Header etc.
BOOST_FUSION_ADAPT_STRUCT(
	fipa::acl::Header,
	(char, id)
	(char, version)
)

BOOST_FUSION_ADAPT_STRUCT(
	fipa::acl::MessageParameter,
	(std::string, name)
	(fipa::acl::ParameterValue, data)
)

BOOST_FUSION_ADAPT_STRUCT(
	fipa::acl::AgentID,
	(std::string, name)
	(std::vector<std::string>, addresses)
	(std::vector<fipa::acl::Resolver>, resolvers)
)

// The final message structure
BOOST_FUSION_ADAPT_STRUCT(
	fipa::acl::Message,
	(fipa::acl::Header, header)
	(std::string, type)
	(std::vector<fipa::acl::MessageParameter>, parameters)
)

namespace fipa
{

//#####################################################
// Utility functions
//#####################################################
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
// IMPORTANT: ACLMessage with following () otherwise, heaps of compiler errors
struct bitefficient_grammar : qi::grammar<Iterator, fipa::acl::Message(), ascii::space_type>
{
	bitefficient_grammar() : bitefficient_grammar::base_type(aclCommunicativeAct, "bitefficient-grammar")
	{
		using qi::lit;
		using qi::lexeme; // prevents character skipping
		
		// Inbuild parser:
		// Encoding is ASCII here
		using ascii::char_;
		using ascii::string;
		using ascii::digit;
		using ascii::alpha;
		using qi::byte_;
		using qi::short_;
		using qi::long_;
		
		// To avoid namespace clashes with boost::bind
		namespace label = qi::labels;
		
		
		// Explanation:
		// at_c<0>(label::_val) = label::_1
		// set the first element (position 0) of the element synthesized attribute,i.e._val, to the parsed value, i.e. _1
		// the synthesized attribute might be a fipa::acl::message, and the element ordering depends on the structure as
		// defined with the BOOST_FUSION_ADAPT_STRUCT definition
		aclCommunicativeAct = header          		[ phoenix::at_c<0>(label::_val) = label::_1 ]
				      >> messageType		[ phoenix::at_c<1>(label::_val) = label::_1 ]
				      >> *messageParameter       [ phoenix::push_back(phoenix::at_c<2>(label::_val), label::_1) ]
				      >> endOfMessage           // No action here
				     ;

		header = messageId  [ phoenix::at_c<0>(label::_val) = label::_1 ] 
			 >> version [ phoenix::at_c<1>(label::_val) = label::_1 ]
			;
		
		// byte_() does only return an unused_type, so if we want to save the value, we either have to assign it directly or we have to use byte_ instead
		messageId = byte_; /*(
                            byte_(0xFA)  [ boost::bind(&fipa::setMessageId,1) ] 
			  | byte_(0xFB)  [ boost::bind(&fipa::setMessageId,2) ]
                          | byte_(0xFC)  [ boost::bind(&fipa::setMessageId,3) ]
			 ); // there is only the used_type for byte_() as attribute, so we cannot use this here 
			 */

		version = byte_; 					 
		endOfMessage %= endOfCollection;
		endOfCollection %= byte_(0x01); 
		
		// we do not support dynamic code tables or user defined values
		// message type is a string
		messageType = predefinedMessageType.alias();
		messageTypeName = binWord.alias();

		// Note: never do a direct assignment like
		// messageParameter = predefinedMessageParameter or you will be getting runtime errors	
		// use messageParameter = predefinedMessageParameter.alias() instead
		messageParameter = predefinedMessageParameter.alias();

		// Converting message type into predefined strings
		predefinedMessageType = byte_(0x01)    [ label::_val = "accept-proposal" ]
					| byte_(0x02)  [ label::_val = "agree" ]  
					| byte_(0x03)  [ label::_val = "cancel" ] 	
					| byte_(0x04)  [ label::_val = "cfp" ]    
					| byte_(0x05)  [ label::_val = "confirm" ]
					| byte_(0x06)  [ label::_val = "disconfirm" ]
					| byte_(0x07)  [ label::_val = "failure" ]
					| byte_(0x08)  [ label::_val = "inform" ]
					| byte_(0x09)  [ label::_val = "inform-if" ]
					| byte_(0x0a)  [ label::_val = "inform-ref" ]
					| byte_(0x0b)  [ label::_val = "not-understood" ]
					| byte_(0x0c)  [ label::_val = "propagate" ]
   					| byte_(0x0d)  [ label::_val = "propose" ]
					| byte_(0x0e)  [ label::_val = "proxy" ] 
					| byte_(0x0f)  [ label::_val = "query-if" ]
					| byte_(0x10)  [ label::_val = "query-ref" ]
					| byte_(0x11)  [ label::_val = "refuse" ]   
					| byte_(0x12)  [ label::_val = "reject-proposal" ]
					| byte_(0x13)  [ label::_val = "request" ]
					| byte_(0x14)  [ label::_val = "request-when" ] 
					| byte_(0x15)  [ label::_val = "request-whenever" ]
					| byte_(0x16)  [ label::_val = "subscribe" ]  
					; 
								

		predefinedMessageParameter = byte_(0x02) [ phoenix::at_c<0>(label::_val) = "sender" ]       >> agentIdentifier [ phoenix::at_c<1>(label::_val) = label::_1 ]   // sender
		/*
					| byte_(0x03) [ phoenix::at_c<0>(label::_val) = "receiver" ]        >> recipientExpr      // receiver 
					| byte_(0x04) [ phoenix::at_c<0>(label::_val) = "content" ]         >> msgContent         // content 
					| byte_(0x05) [ phoenix::at_c<0>(label::_val) = "reply-with" ]      >> replyWithParam     // reply-with
					| byte_(0x06) [ phoenix::at_c<0>(label::_val) = "reply-by" ]        >> replyByParam       // reply-by 
					| byte_(0x07) [ phoenix::at_c<0>(label::_val) = "in-reply-to" ]     >> inReplyToParam     // in-reply-to 
					| byte_(0x08) [ phoenix::at_c<0>(label::_val) = "reply-to" ]        >> replyToParam       // reply-to   
					| byte_(0x09) [ phoenix::at_c<0>(label::_val) = "language" ]        >> language           // language  
					| byte_(0x0a) [ phoenix::at_c<0>(label::_val) = "encoding" ]        >> encoding           // encoding 
					| byte_(0x0b) [ phoenix::at_c<0>(label::_val) = "ontology" ]        >> ontology           // ontology
					| byte_(0x0c) [ phoenix::at_c<0>(label::_val) = "protocol" ]        >> protocol           // protocol
					| byte_(0x0d) [ phoenix::at_c<0>(label::_val) = "conversation-id" ] >> conversationId    // conversation-id
	*/				; 
		
		agentIdentifier = byte_(0x02) >> agentName 		[ phoenix::at_c<0>(label::_val) = label::_1 ]
				 >> -addresses  	   		[ phoenix::at_c<1>(label::_val) = label::_1 ]
				 >> -resolvers 	           		[ phoenix::at_c<2>(label::_val) = label::_1 ]
				 //>> *(userDefinedParameter) 
				>> endOfCollection;				
		agentName = binWord.alias();
		addresses = byte_(0x02) >> urlCollection 		[ label::_val = label::_1 ];
		resolvers = byte_(0x03) >> *agentIdentifier 		[ phoenix::push_back(label::_val, label::_1) ]
					   >> endOfCollection;

		userDefinedParameter = byte_(0x04) >> binWord 		[ phoenix::at_c<0>(label::_val) = label::_1 ]
					 >> binExpression     		[ phoenix::at_c<1>(label::_val) = "TODO:binExpression" ]  
					;
		
		urlCollection = *url 					[ phoenix::push_back(label::_val, label::_1) ]
				 >> endOfCollection;
		url = binWord.alias();

		recipientExpr = *agentIdentifier >> endOfCollection;
		msgContent = binString.alias();	

		replyWithParam = binExpression.alias();
		replyByParam = binDateTimeToken.alias();
		inReplyToParam = binExpression.alias();
		replyToParam = recipientExpr.alias();
		language = binExpression.alias();
		encoding = binExpression.alias();
		ontology = binExpression.alias();
		protocol = binWord.alias();
		conversationId = binExpression.alias();
	

		binWord = ( ( byte_(0x10) >> word 		[ label::_val = label::_1 ]
				>> byte_(0x00) )
		        | byte_(0x11) >> index   		[ label::_val = extractFromCodetable(label::_1) ]
			);

		binNumber %= ( byte_(0x12) >> digits )          // Decimal numbers
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
		
		// Index is a pointer to code table entry and its size (in bits) depends on the code table size. 
		// If the code table size is 256 entries, the size of the index is one byte;
		//  otherwise its size is two bytes (represented in network byte order).
		index = byte_ | short_;
		byteSeq %= *byte_;
		
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

		word = (char_ - wordExceptionsStart )  		[ label::_val += label::_1 ]
			 >> *(char_ - wordExceptionsGeneral)    [ label::_val += label::_1 ]
			;  //TODO: TESTING

		wordExceptionsStart %= wordExceptionsGeneral
                                | char_('#') 
				| char_('0','9')
				| char_('-')
				| char_('@')
				;
		wordExceptionsGeneral %= char_('\x00','\x20') 
				| char_('(')
                                | char_(')')
				;
				
		fipaString %= stringLiteral | byteLengthEncodedString;
		stringLiteral %= char_('\\') >> *( ( char_ - char_('\\') ) | (char_('\\') >> char_('\"') ) ) >> char_('\"'); // TODO: REQUIRES VERYFICATION
		byteLengthEncodedString %= char_('#') >> +digit >> char_('\"') >> byteSeq; // REQUIRES TESTING

		codedNumber %= byte_; // two numbers in one byte - padding 00 if coding only one number
		typeDesignator %= alpha;
	}
	
	// Define rules as follows
	// qi::rule<Iterator, synthesized_attribute(inherited_attribute>)> r;
	// synthesized = type of output value
	// inherited = actual type the parser gives you
	qi::rule<Iterator, fipa::acl::Message(), ascii::space_type> aclCommunicativeAct;
	qi::rule<Iterator> message;
	qi::rule<Iterator, fipa::acl::Header()> header;
	qi::rule<Iterator, char()> messageId;
	qi::rule<Iterator, char()> version;
	qi::rule<Iterator> endOfMessage;
	qi::rule<Iterator> endOfCollection;

	qi::rule<Iterator, std::string() > messageType;
	qi::rule<Iterator, std::string() > userDefinedMessageType;
	qi::rule<Iterator, std::string() > messageTypeName;

	qi::rule<Iterator, fipa::acl::MessageParameter() > messageParameter;
	qi::rule<Iterator, fipa::acl::MessageParameter() > userDefinedMessageParameter;
	qi::rule<Iterator, fipa::acl::MessageParameter() > predefinedMessageParameter;
	
	qi::rule<Iterator, std::string() > parameterName;
	qi::rule<Iterator, std::string() > parameterValue;

	qi::rule<Iterator, std::string() > predefinedMessageType;
	
	qi::rule<Iterator, fipa::acl::AgentID()> agentIdentifier;
	qi::rule<Iterator, std::string() > agentName;
	qi::rule<Iterator, std::vector<std::string>() > addresses;
	qi::rule<Iterator, std::vector<fipa::acl::Resolver>() > resolvers;
 	qi::rule<Iterator, fipa::acl::MessageParameter() > userDefinedParameter;
	
	qi::rule<Iterator, std::vector<std::string>() > urlCollection;
	qi::rule<Iterator, std::string() > url;
	qi::rule<Iterator, std::vector<fipa::acl::Resolver>() > agentIdentifierCollection;
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
	
	qi::rule<Iterator, std::string()> binWord;
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
	qi::rule<Iterator, unsigned short() > index;
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

	qi::rule<Iterator, std::string() > word;
	qi::rule<Iterator> wordExceptionsStart;
	qi::rule<Iterator> wordExceptionsGeneral;
	qi::rule<Iterator> fipaString;
	qi::rule<Iterator> stringLiteral;
	qi::rule<Iterator> byteLengthEncodedString;
	qi::rule<Iterator> codedNumber;
	qi::rule<Iterator> typeDesignator;

};

} // end namespace acl
} // end namespace fipa


