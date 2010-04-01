/**
 *
 * \file grammar_bitefficient.h
 * \author Thomas Roehr, thomas.roehr@dfki.de
 * \brief This grammar represents the bitefficient message specification
 * of the Foundation for Intelligent Physical Agents (FIPA at http://www.fipa.org)
 * 
 * \version 0.1
 *  - parses valid messages
 *  - extract some field information from messages
 */

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_repeat.hpp>

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

typedef boost::variant<boost::uint_least8_t,boost::uint_least16_t, boost::uint_least32_t> LengthValue;


// Bytesequence - whereas the encoding is defined by a natural number, i.e. 
// postprocessing has to be performed 
typedef struct
{
	std::string encoding;
	fipa::acl::LengthValue length;
	std::string bytes;
} ByteSequence;

struct AgentID;
//struct MessageParameter;

typedef boost::recursive_wrapper<AgentID> Resolver;

// To avoid dealing with circular dependecies we use a 'similar' definition to ParameterValue 
typedef boost::variant<std::string, fipa::acl::Resolver, std::vector<fipa::acl::Resolver>, fipa::acl::ByteSequence > UserDefinedParameterValue;

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

typedef boost::variant<std::string, fipa::acl::AgentID, std::vector<fipa::acl::AgentID>, fipa::acl::ByteSequence > ParameterValue;

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

class AgentIDPrinter
{
	public:
	AgentIDPrinter() {}
	
	void print(const fipa::acl::AgentID& aid)
	{
				printf("\t\tname: %s\n", aid.name.c_str());
				for(int a=0; a < aid.addresses.size(); a++)
				{	
					printf("\t\taddresses: %s\n", aid.addresses[a].c_str());
				}
				printf("\t\tresolvers:\n");
				for(int r=0; r < aid.resolvers.size(); r++)
				{
					fipa::acl::AgentID resolver = aid.resolvers[r].get();
					print(resolver);
				}

	}

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

			if( (msg.header.id & ! 0xfa) == 0) printf("\tid is FA\n");
			else if( (msg.header.id & ! 0xfb) == 0 ) printf("\tid is FB\n");
			else if( (msg.header.id & ! 0xfc) == 0 ) printf("\tid is FC\n"); 
			else printf("\tWARNING: id is unknown\n");
	

		printf("version:                  %x\n", msg.header.version); 
		printf("performative(type):       %s\n", msg.type.c_str());
		
		printf("msg-parameters count:     %d\n", msg.parameters.size());
		for(int i=0; i < msg.parameters.size(); i++)
		{
			MessageParameter mp = msg.parameters[i];
			printf("\tparameter #%d:  %s\n",i, mp.name.c_str()); 
		
			if(mp.name == "sender")
			{
				AgentIDPrinter aidPrinter;
				fipa::acl::AgentID& aid = boost::get<fipa::acl::AgentID>(mp.data);
				aidPrinter.print(aid);
			}
			
			if(mp.name == "content")
			{	
				fipa::acl::ByteSequence bs = boost::get<fipa::acl::ByteSequence>(mp.data);
				printf("encoding: %s\n", bs.encoding.c_str());
				printf("content: %s\n", bs.bytes.c_str());
			}
			
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
	fipa::acl::ByteSequence,
	(std::string, encoding)
	(fipa::acl::LengthValue, length)
	(std::string, bytes)
)

BOOST_FUSION_ADAPT_STRUCT(
	fipa::acl::MessageParameter,
	(std::string, name)
	(fipa::acl::ParameterValue, data)
)

BOOST_FUSION_ADAPT_STRUCT(
	fipa::acl::UserDefinedParameter,
	(std::string, name)
	(fipa::acl::UserDefinedParameterValue, data)
)

BOOST_FUSION_ADAPT_STRUCT(
	fipa::acl::AgentID,
	(std::string, name)
	(std::vector<std::string>, addresses)
	(std::vector<fipa::acl::Resolver>, resolvers)
	(std::vector<fipa::acl::UserDefinedParameter>, parameters)
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


			/*switch(index)
			{
				case 1:
					return std::string("ONE");
					break;
				case 2:
					return std::string("TWO");
					break;
			}*/

			return std::string("Codetable currently unsupported");
		}
	
	};

	phoenix::function<extractFromCodetableImpl> extractFromCodetable;

	struct convertToStringImpl
	{
		template <typename T>
		struct result
		{
			typedef std::string type;
		};

		// for std::vector<char>
		template <typename T>
		std::string operator()(T arg) const
		{
			return std::string(arg.begin(), arg.end());
		}

		std::string operator()(fipa::acl::ByteSequence arg) const
		{
			// perform some encoding stuff here
			// using arg.encoding
			return arg.bytes;
		}


	};

	

	phoenix::function<convertToStringImpl> convertToString;

	struct setSequenceLengthImpl
	{
		template <typename T>
		struct result
		{
			typedef void type;
		};

		template <typename T>
		void operator() (boost::uint32_t* lengthVar, T arg) const
		{
			lengthVar = 0;

			if( boost::is_convertible<T,const boost::uint32_t>::value )
			{
				*lengthVar = arg;
			}
			
			return;
		}
	};
	
	phoenix::function<setSequenceLengthImpl> setSequenceLength;
	/*
	typedef boost::variant<std::vector<boost::uint8_t>,
					       std::vector<boost::uint16_t>,
					       std::vector<boost::uint32_t>,
					       std::vector<boost::uint64_t> encodedBytes;
	struct createByteSequenceImpl
	{
		template <typename T>
		struct result
 		{
			typedef encodedBytes type;
		};	

		template <typename T>
		encodedBytes operator()(T arg) const
		{
			
		}
	};
	
	phoenix::function<createByteSequenceImpl> createByteSequence;
*/

namespace acl
{

template <typename Iterator>
// IMPORTANT: ACLMessage with following () otherwise, heaps of compiler errors
struct bitefficient_grammar : qi::grammar<Iterator, fipa::acl::Message(), ascii::space_type>
{
	bitefficient_grammar() : bitefficient_grammar::base_type(aclCommunicativeAct, "bitefficient-grammar")
	{
		using qi::on_error;
		using qi::fail;
		using phoenix::construct;
		using phoenix::val;

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
		aclCommunicativeAct = header          		 [ phoenix::at_c<0>(label::_val) = label::_1 ]
				      >> messageType		 [ phoenix::at_c<1>(label::_val) = label::_1 ]
				      >> *messageParameter       [ phoenix::push_back(phoenix::at_c<2>(label::_val), label::_1) ]
				      >> endOfMessage           // No action here
				     ;

		header = messageId  [ phoenix::at_c<0>(label::_val) = label::_1 ] 
			 >> version [ phoenix::at_c<1>(label::_val) = label::_1 ]
			;
		
		// byte_() does only return an unused_type, so if we want to save the value, we either have to assign it directly
		messageId = byte_(0xFA)  [ label::_val = 0xfa ] 
			  | byte_(0xFB)  [ label::_val = 0xfb ]
                          | byte_(0xFC)  [ label::_val = 0xfc ]
			 ; 
			 

		version = byte_; 					 
		endOfMessage %= endOfCollection;
		endOfCollection %= byte_(0x01); 
		
		// we do not support dynamic code tables or user defined values
		// message type is a string
		// the types in the rule definition have to match to apply alias()
		messageType = predefinedMessageType | userDefinedMessageType;

		userDefinedMessageType = byte_(0x00)
				       >> messageTypeName 	[ label::_val = label::_1 ]
				       ;

		
		messageTypeName = binWord.alias();

		// Note: never do a direct assignment like
		// messageParameter = predefinedMessageParameter or you will be getting runtime errors	
		// use messageParameter = predefinedMessageParameter.alias() instead
		messageParameter = predefinedMessageParameter | userDefinedMessageParameter;

		userDefinedMessageParameter = byte_(0x00)
					    >> parameterName 		[ phoenix::at_c<0>(label::_val) = label::_1 ]
					    >> parameterValue           [ phoenix::at_c<1>(label::_val) = label::_1 ]
					    ;

		parameterName = binWord.alias();
		parameterValue = binExpression.alias();

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
								
		// predefinedMessageParamter uses a boost::variant
		predefinedMessageParameter = byte_(0x02) [ phoenix::at_c<0>(label::_val) = "sender" ]       >> agentIdentifier [ phoenix::at_c<1>(label::_val) = label::_1 ]    // sender
					| byte_(0x03) [ phoenix::at_c<0>(label::_val) = "receiver" ]        >> recipientExpr   [ phoenix::at_c<1>(label::_val) = label::_1 ]   // receiver 
					| byte_(0x04) [ phoenix::at_c<0>(label::_val) = "content" ]         >> msgContent      [ phoenix::at_c<1>(label::_val) = label::_1 ]   // content 
					| byte_(0x05) [ phoenix::at_c<0>(label::_val) = "reply-with" ]      >> replyWithParam  [ phoenix::at_c<1>(label::_val) = label::_1 ]   // reply-with
					| byte_(0x06) [ phoenix::at_c<0>(label::_val) = "reply-by" ]        >> replyByParam    [ phoenix::at_c<1>(label::_val) = "todo:binDateTimeToken" ]  // reply-by 
					| byte_(0x07) [ phoenix::at_c<0>(label::_val) = "in-reply-to" ]     >> inReplyToParam  [ phoenix::at_c<1>(label::_val) = label::_1 ]  // in-reply-to 
					| byte_(0x08) [ phoenix::at_c<0>(label::_val) = "reply-to" ]        >> replyToParam    [ phoenix::at_c<1>(label::_val) = label::_1 ] // reply-to   
					| byte_(0x09) [ phoenix::at_c<0>(label::_val) = "language" ]        >> language        [ phoenix::at_c<1>(label::_val) = label::_1 ] // language  
					| byte_(0x0a) [ phoenix::at_c<0>(label::_val) = "encoding" ]        >> encoding        [ phoenix::at_c<1>(label::_val) = label::_1 ] // encoding 
					| byte_(0x0b) [ phoenix::at_c<0>(label::_val) = "ontology" ]        >> ontology        [ phoenix::at_c<1>(label::_val) = label::_1 ] // ontology
					| byte_(0x0c) [ phoenix::at_c<0>(label::_val) = "protocol" ]        >> protocol        [ phoenix::at_c<1>(label::_val) = label::_1 ] // protocol
					| byte_(0x0d) [ phoenix::at_c<0>(label::_val) = "conversation-id" ] >> conversationId  [ phoenix::at_c<1>(label::_val) = label::_1 ] // conversation-id
					; 
		
		agentIdentifier = byte_(0x02) >> agentName 		[ phoenix::at_c<0>(label::_val) = label::_1 ]
				 >> -addresses  	   		[ phoenix::at_c<1>(label::_val) = label::_1 ]
				 >> -resolvers 	           		[ phoenix::at_c<2>(label::_val) = label::_1 ]
				 >> *userDefinedParameter		[ phoenix::push_back(phoenix::at_c<3>(label::_val), label::_1) ] 
				 >> endOfCollection;				

		agentName = binWord.alias();
		addresses = byte_(0x02) >> urlCollection 		[ label::_val = label::_1 ];
		resolvers = byte_(0x03) >> *agentIdentifier 		[ phoenix::push_back(label::_val, label::_1) ]
					   >> endOfCollection;

		userDefinedParameter = byte_(0x04) >> binWord 		[ phoenix::at_c<0>(label::_val) = label::_1 ]
					 >> binExpression     		[ phoenix::at_c<1>(label::_val) = label::_1 ]  
					;
		
		urlCollection = *url 					[ phoenix::push_back(label::_val, label::_1) ]
				 >> endOfCollection;
		url = binWord.alias();

		recipientExpr = *agentIdentifier 			[ phoenix::push_back(label::_val, label::_1) ]
				>> endOfCollection;
		
		// Make sure rule definition matches for alias()
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

		binString = ( byte_(0x14) >> ( stringLiteralTerminated 
					   | byteLengthEncodedStringTerminated )  		[ label::_val = label::_1 ])
			  // string literal from code table
			  | ( byte_(0x15) >> index 			        [ phoenix::at_c<2>(label::_val) = extractFromCodetable(label::_1) ])            
			  // The byte length will be stored in the rule local variable (label::_a) and then forwarded to the qi::repeat instruction
			  | ( byte_(0x16) >> len8                       	[ label::_a = label::_1 ]
					  >> byteSeq(label::_a)			[ phoenix::at_c<2>(label::_val) = label::_1 ] )  // new byteLengthEncoded string 
			  | ( byte_(0x17) >> len16 			        [ label::_a = label::_1 ]
				          >> byteSeq(label::_a)			[ phoenix::at_c<2>(label::_val) = label::_1 ] )
			  | ( byte_(0x18) >> index                              [ phoenix::at_c<2>(label::_val) = extractFromCodetable(label::_1) ]) // byteLengthEncoded from code table
			  | ( byte_(0x19) >> len32 			        [ label::_a = label::_1 ]
                                          >> byteSeq(label::_a)			[ phoenix::at_c<2>(label::_val) = label::_1 ] )
			  ;         

		binDateTimeToken %= ( byte_(0x20) >> binDate )		            // Absolute time  
				| ( byte_(0x21) >> binDate )		            // Relative time (+)
				| ( byte_(0x22) >> binDate )		            // Relative time (-)
				| ( byte_(0x24) >> binDate >> typeDesignator )      // Absolute time   
				| ( byte_(0x25) >> binDate >> typeDesignator )      // Relative time (+)
				| ( byte_(0x26) >> binDate >> typeDesignator );     // Relative time (-)

		binDate %= year >> month >> day >> hour >> minute >> second >> millisecond;
		
		binExpression = binExpr				[ label::_val = label::_1 ] 
			      | byte_(0xFF) >> binString        [ label::_val = convertToString(label::_1) ] 
			      ;

		binExpr = binWord				[ label::_val = label::_1 ]
			| binString				[ label::_val = convertToString(label::_1) ] 
			| binNumber				[ label::_val = "todo:binNumber" ]
			// Every expression can look like the following of "(+ (-1 2) 3)"
			| (exprStart 				[ label::_val = "(" , label::_val += label::_1 ]
			  >> *binExpr				[ label::_val += label::_1 ]
			  >> exprEnd 				[ label::_val += ")", label::_val += label::_1 ]
			  )
			;

		exprStart = ( byte_(0x60) ) 					 [ label::_val = "" ]
			  | ( byte_(0x70) >> word       			 [ label::_val = label::_1 ]
					  >> byte_(0x00) )
			  | ( byte_(0x71) >> index      			 [ label::_val = extractFromCodetable(label::_1)] )
			  | ( byte_(0x72) >> digits     			 [ label::_val = "todo:digits" ] )
			  | ( byte_(0x73) >> digits     			 [ label::_val = "todo:digits" ] ) 
			  | ( byte_(0x74) >> ( stringLiteralTerminated 
					   | byteLengthEncodedStringTerminated ) [ label::_val = convertToString(label::_1) ] )
			  | ( byte_(0x75) >> index      			 [ label::_val = extractFromCodetable(label::_1)])
			  | ( byte_(0x76) >> len8 				 [ label::_a = label::_1 ]
					  >> fipaString(label::_a)		 [ label::_val = convertToString(label::_1) ]) 
			  | ( byte_(0x77) >> len16 				 [ label::_a = label::_1 ] 
					  >> fipaString(label::_a)		 [ label::_val = convertToString(label::_1) ])
			  | ( byte_(0x78) >> len32 				 [ label::_a = label::_1 ]
				          >> fipaString(label::_a)		 [ label::_val = convertToString(label::_1) ])
			  | ( byte_(0x79) >> index 				 [ label::_val = extractFromCodetable(label::_1)])
			 ;

		exprEnd = ( byte_(0x40) ) 					 [ label::_val = "" ]
			| ( byte_(0x50) >> word 	 			 [ label::_val = label::_1 ]
					>> byte_(0x00) )
			| ( byte_(0x51) >> index      				 [ label::_val = extractFromCodetable(label::_1)])
			| ( byte_(0x52) >> digits 				 [ label::_val = "todo:digits" ] )
			| ( byte_(0x53) >> digits 				 [ label::_val = "todo:digits" ] )
			| ( byte_(0x54) >> ( stringLiteralTerminated 
					   | byteLengthEncodedStringTerminated ) [ label::_val = convertToString(label::_1) ])
			| ( byte_(0x55) >> index      				 [ label::_val = extractFromCodetable(label::_1)]) 
			| ( byte_(0x56) >> len8       				 [ label::_a = label::_1 ]
					>> fipaString(label::_a)		 [ label::_val = convertToString(label::_1)] )
			| ( byte_(0x57) >> len16      				 [ label::_a = label::_1 ]
					>> fipaString(label::_a)    		 [ label::_val = convertToString(label::_1)])
			| ( byte_(0x58) >> len32      				 [ label::_a = label::_1 ]
					>> fipaString(label::_a)		 [ label::_val = convertToString(label::_1)])
			| ( byte_(0x59) >> index      		                 [ label::_val = extractFromCodetable(label::_1)])
			;
		
		// Index is a pointer to code table entry and its size (in bits) depends on the code table size. 
		// If the code table size is 256 entries, the size of the index is one byte;
		//  otherwise its size is two bytes (represented in network byte order).
		index = byte_ | short_;

		// Bytesequences can only be interpreted if the sequenceLength value is set correctly 
		// TODO: check value and reset
		byteSeq = qi::repeat(label::_r1)[byte_]	[ label::_val = convertToString(label::_1) ]
			;
		
		len8 = byte_;
		len16 = short_;
		len32 = long_;
	
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
		
		// pass the local length variable down to the rule using parent rules variable  _r1			
		fipaString = (stringLiteral | byteLengthEncodedString(label::_r1) );

		
		// Order of statement is relevant here, since the character \ needs to be matched
		// first -- matching is greedy with char_ otherwise
		stringLiteral = char_('"') 
			       >> * (( char_("\\") >> char_('"') ) 	[ phoenix::at_c<2>(label::_val) += "\"" ]
				  | (char_ - char_('"') ) 		[ phoenix::at_c<2>(label::_val) += label::_1 ]
				)
			      >> char_('"'); 

		stringLiteralTerminated = stringLiteral 		[ label::_val = label::_1 ]
					>> byte_(0x00)
					;
	
		byteLengthEncodedStringHeader = char_('#')
					      >> + digit 	[ label::_val += label::_1 ]
					      >> char_('"')
					      ;

		// Digits tell the byte encoding
		// label::_r1 is an inherited local variable from the parent rule
		byteLengthEncodedString = byteLengthEncodedStringHeader 		        [ phoenix::at_c<0>(label::_val) += label::_1 ]
					>> qi::repeat(label::_r1)[byte_]     			[ phoenix::at_c<2>(label::_val) = convertToString(label::_1) ]
					;

		byteLengthEncodedStringTerminated = byteLengthEncodedStringHeader 		[ phoenix::at_c<0>(label::_val) += label::_1 ]
						>> * (byte_ - byte_(0x00))			[ phoenix::at_c<2>(label::_val) += label::_1 ]
						>> byte_(0x00)
						;

		codedNumber %= byte_; // two numbers in one byte - padding 00 if coding only one number
		typeDesignator %= alpha;


		on_error<fail>
		(
		    //aclCommunicativeAct,
		    byteLengthEncodedString,
		    std::cout
			<< val("Error: expecting ")
			<< label::_4 			   // what failed?
			<< val(" here: \"")
			<< construct<std::string>(label::_3,label::_2)
			<< val("\"")
			<< std::endl
                );
		
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
 	qi::rule<Iterator, fipa::acl::UserDefinedParameter() > userDefinedParameter;
	
	qi::rule<Iterator, std::vector<std::string>() > urlCollection;
	qi::rule<Iterator, std::string() > url;
	qi::rule<Iterator, std::vector<fipa::acl::AgentID>() > recipientExpr;
	qi::rule<Iterator, fipa::acl::ByteSequence() > msgContent;
	qi::rule<Iterator, std::string() > replyWithParam;
	qi::rule<Iterator> replyByParam;
	qi::rule<Iterator, std::string() > inReplyToParam;
	qi::rule<Iterator, std::vector<fipa::acl::AgentID>() > replyToParam;
	qi::rule<Iterator, std::string() > language;
	qi::rule<Iterator, std::string() > encoding;
	qi::rule<Iterator, std::string() > ontology;
	qi::rule<Iterator, std::string() > protocol;
	qi::rule<Iterator, std::string() > conversationId;
	
	qi::rule<Iterator, std::string()> binWord;
	qi::rule<Iterator> binNumber;
	qi::rule<Iterator, std::string() > digits; 
	qi::rule<Iterator, fipa::acl::ByteSequence(), qi::locals<boost::uint_least32_t> > binString;
	qi::rule<Iterator> binDateTimeToken;
	qi::rule<Iterator> binDate;
	qi::rule<Iterator, std::string() > binExpression;
	qi::rule<Iterator, std::string() > binExpr;
	qi::rule<Iterator, std::string(), qi::locals<boost::uint_least32_t> > exprStart;
	qi::rule<Iterator, std::string(), qi::locals<boost::uint_least32_t> > exprEnd;

	//qi::symbols<char, qi::rule<Iterator> > exprKeyword;

	qi::rule<Iterator, std::string(boost::uint32_t) > byteSeq;
	qi::rule<Iterator, unsigned short() > index;
	qi::rule<Iterator, boost::uint_least8_t() > len8;
	qi::rule<Iterator, boost::uint_least16_t() > len16;
	qi::rule<Iterator, boost::uint_least32_t() > len32;
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
	qi::rule<Iterator, fipa::acl::ByteSequence(boost::uint32_t) > fipaString;
	qi::rule<Iterator, fipa::acl::ByteSequence() > stringLiteral;
	qi::rule<Iterator, fipa::acl::ByteSequence() > stringLiteralTerminated;
	qi::rule<Iterator, std::string() > byteLengthEncodedStringHeader;
	qi::rule<Iterator, fipa::acl::ByteSequence(boost::uint32_t) > byteLengthEncodedString;
	qi::rule<Iterator, fipa::acl::ByteSequence() > byteLengthEncodedStringTerminated;
	qi::rule<Iterator> codedNumber;
	qi::rule<Iterator> typeDesignator;

};

} // end namespace acl
} // end namespace fipa


