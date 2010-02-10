/**
 *
 * \file grammar_bitefficient.h
 * \author Thomas Roehr, thomas.roehr@dfki.de
 */

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
	std::string type;
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
		printf("id:          %x\n", msg.header.id);
		printf("version:     %x\n", msg.header.version); 
		printf("type:        %s\n", msg.type.c_str());
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

// The final message structure
BOOST_FUSION_ADAPT_STRUCT(
	fipa::acl::Message,
	(fipa::acl::Header, header)
	(std::string, type)
)

namespace fipa
{
	struct storeVersion
	{
		void operator()(char const& c, qi::unused_type, qi::unused_type) const
		{
			printf("Version: %x \n", c);
		}
	};

	struct storeMessageId
	{	
		void operator()(char const& c, qi::unused_type, qi::unused_type) const
		{

			printf("MessageId: %x \n", c);
		}

	};

	void setMessageId(int i)
	{
		printf("Set MessageId to: %i\n", i);

	}

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
		using phoenix::at_c;
		
		// Explanation:
		// at_c<0>(label::_val) = label::_1
		// set the first element (position 0) of the element synthesized attribute,i.e._val, to the parsed value, i.e. _1
		// the synthesized attribute might be a fipa::acl::message, and the element ordering depends on the structure as
		// defined with the BOOST_FUSION_ADAPT_STRUCT definition
		aclCommunicativeAct = header          		[ at_c<0>(label::_val) = label::_1 ]
					>> messageType		[ at_c<1>(label::_val) = label::_1 ]
					>> *messageParameter
					>> endOfMessage
				     ;

		header = messageId  [ at_c<0>(label::_val) = label::_1 ] 
			 >> version [ at_c<1>(label::_val) = label::_1]
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
		messageType = predefinedMessageType  [ label::_val = label::_1 ] 
			    | userDefinedMessageType [ label::_val = label::_1 ]
 			   ;
		userDefinedMessageType = byte_(0x00) >> messageTypeName [ label::_val = "messagTypeName" ];
		messageTypeName %= binWord;
	
		messageParameter %= predefinedMessageParameter | userDefinedMessageParameter;
		userDefinedMessageParameter %= byte_(0x00) >> parameterName >> parameterValue;
		parameterName %= binWord;

		parameterValue %= binExpression;
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

		word %= (char_ - wordExceptionsStart ) >> *(char_ - wordExceptionsGeneral);  //TODO: TESTING
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
	qi::rule<Iterator> messageTypeName;
	qi::rule<Iterator> messageParameter;
	qi::rule<Iterator> userDefinedMessageParameter;
	
	qi::rule<Iterator> parameterName;
	qi::rule<Iterator> parameterValue;

	qi::rule<Iterator, std::string() > predefinedMessageType;
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


