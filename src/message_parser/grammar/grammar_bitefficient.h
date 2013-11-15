#ifndef _FIPAACL_GRAMMAR_BITEFFICIENT_H_
#define _FIPAACL_GRAMMAR_BITEFFICIENT_H_

#include <fipa_acl/message_parser/grammar/grammar_common.h>

using namespace fipa::acl::grammar;

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
	fipa::acl::message::Parameter,
	(std::string, name)
	(fipa::acl::message::ParameterValue, data)
)

BOOST_FUSION_ADAPT_STRUCT(
	fipa::acl::UserDefinedParameter,
	(std::string, name)
	(fipa::acl::UserDefinedParameterValue, data)
)

BOOST_FUSION_ADAPT_STRUCT(
	fipa::acl::AgentIdentifier,
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
	(std::vector<fipa::acl::message::Parameter>, parameters)
)

namespace fipa
{

//#####################################################
// Utility functions
//#####################################################
	// In order to use functions as semantic actions
	// lazy evaluation is required	
  
namespace acl {
namespace bitefficient {

// To avoid namespace clashes with boost::bind
namespace label = qi::labels;

template<typename Iterator>
struct EndOfCollection : qi::grammar<Iterator, qi::unused_type>
{
    EndOfCollection() : EndOfCollection::base_type(eof_collection_rule, "EndOfCollection-bitefficient_grammar")
    {
        eof_collection_rule = qi::byte_(0x01);

        FIPA_DEBUG_RULE(eof_collection_rule);
    }
 
    qi::rule<Iterator, qi::unused_type> eof_collection_rule;
};

template<typename Iterator>
struct BinStringNoCodetable : qi::grammar<Iterator, fipa::acl::ByteSequence(), qi::locals<boost::uint_least32_t> >
{
    BinStringNoCodetable() : BinStringNoCodetable::base_type(bin_string_no_codetable_rule, "BinStringNoCodetable-bitefficient_grammar")
    {
        using namespace qi;

        bin_string_no_codetable_rule = ( byte_(0x14) >> nullTerminatedString 	[ label::_val = label::_1 ])
            // The byte length will be stored in the rule local variable (label::_a) and then forwarded to the qi::repeat instruction
            | ( byte_(0x16) >> len8                       	[ label::_a = label::_1 ]
                  	  >> byteSeq(label::_a)			[ phoenix::at_c<2>(label::_val) = label::_1] )  // new byteLengthEncoded string 
            | ( byte_(0x17) >> len16 			        [ label::_a = label::_1 ]
                            >> byteSeq(label::_a)			[ phoenix::at_c<2>(label::_val) = label::_1 ] )
            | ( byte_(0x19) >> len32 			        [ label::_a = label::_1 ]
                            >> byteSeq(label::_a)			[ phoenix::at_c<2>(label::_val) = label::_1 ] )
            ;         

	byteSeq = qi::repeat(label::_r1)[byte_]	[ label::_val = convertToCharVector(label::_1) ]
		;
	FIPA_DEBUG_RULE(bin_string_no_codetable_rule);
    }

    qi::rule<Iterator, fipa::acl::ByteSequence(), qi::locals<boost::uint_least32_t> > bin_string_no_codetable_rule;
    Len8<Iterator> len8;
    Len16<Iterator> len16;
    Len32<Iterator> len32;
    qi::rule<Iterator, std::vector<unsigned char>(boost::uint_least32_t) > byteSeq;
    NullTerminatedString<Iterator> nullTerminatedString;
};

template<typename Iterator>
struct BinStringCodetable : qi::grammar<Iterator, fipa::acl::ByteSequence(), qi::locals<boost::uint_least32_t> >
{
    BinStringCodetable() : BinStringCodetable::base_type(bin_string_codetable_rule, "BinStringCodetable-bitefficient_grammar")
    {
        using namespace qi;

        bin_string_codetable_rule =
            // string literal from code table
            ( byte_(0x15) >> index 	  [ phoenix::at_c<2>(label::_val) = extractFromCodetable(label::_1) ])            
            | ( byte_(0x18) >> index        [ phoenix::at_c<2>(label::_val) = extractFromCodetable(label::_1) ]) // byteLengthEncoded from code table
            ;

	FIPA_DEBUG_RULE(bin_string_codetable_rule);
    }

    qi::rule<Iterator, fipa::acl::ByteSequence(), qi::locals<boost::uint_least32_t> > bin_string_codetable_rule;
    Index<Iterator> index;
};


template<typename Iterator>
struct BinString : qi::grammar<Iterator, fipa::acl::ByteSequence() >
{
    BinString() : BinString::base_type(bin_string_rule, "BinString-bitefficient_grammar")
    {
        bin_string_rule = ( binStringNoCodetable | binStringCodetable) [ label::_val = label::_1];

	FIPA_DEBUG_RULE(bin_string_rule);
    }

    qi::rule<Iterator, fipa::acl::ByteSequence()> bin_string_rule;
    BinStringNoCodetable<Iterator> binStringNoCodetable;
    BinStringCodetable<Iterator> binStringCodetable;
};


template<typename Iterator>
struct BinNumber : qi::grammar<Iterator, std::string()>
{
    BinNumber() : BinNumber::base_type(bin_number_rule, "BinNumber-bitefficient_grammar")
    {
        using qi::byte_;
	// Decimal number 0x12
	// Hex number 0x13
	// Hexadecimal number are converted to int and then back
	bin_number_rule = ( byte_(0x12) >> digits 	[ label::_val = label::_1 ] )
		  | ( byte_(0x13) >> digits 	[ label::_val = convertDigitsToHex(label::_1) ]
		  );

	FIPA_DEBUG_RULE(bin_number_rule);
    }

    qi::rule<Iterator, std::string()> bin_number_rule;
    Digits<Iterator> digits;
};

template <typename Iterator>
struct BinDateTime : qi::grammar<Iterator, fipa::acl::DateTime()>
{
    BinDateTime() : BinDateTime::base_type(bin_date_time_rule, "BinDateTime-bitefficient_grammar")
    {
        using qi::byte_;
        using encoding::alpha;

	// Absolute time, relative +/-
	// identifier indicate if typeDesignator follows or not
	// NOTE: When parser fails, make sure the data forwarded to the parser really(!) contains
	// all characters. Since 0x20 is a special character in normal ASCII some ways of reading the data
	// or using std::copy might lead to an unexpected failure, since these special characters will be discarded
	bin_date_time_rule = ( byte_(0x20) 			[ phoenix::at_c<0>(label::_val) = ' ' ]
			    >> binDate				[ phoenix::at_c<1>(label::_val) = label::_1 ]
			   )
			// Relative time (+)
			| ( byte_(0x21) 			[ phoenix::at_c<0>(label::_val) = '+' ]
			   >> binDate				[ phoenix::at_c<1>(label::_val) = label::_1 ]
                          )
			// Relative time (-)
			| ( byte_(0x22)				[ phoenix::at_c<0>(label::_val) = '-' ]
			   >> binDate				[ phoenix::at_c<1>(label::_val) = label::_1 ]
                          )
			// Absolute time   
			| ( byte_(0x24)				[ phoenix::at_c<0>(label::_val) = ' ' ] 
				>> binDate			[ phoenix::at_c<1>(label::_val) = label::_1 ]
				>> typeDesignator 		[ phoenix::at_c<2>(label::_val) = label::_1 ]
				) 
			// Relative time (+)
			| ( byte_(0x25) 			[ phoenix::at_c<0>(label::_val) = '+' ]
				>> binDate			[ phoenix::at_c<1>(label::_val) = label::_1 ]
				>> typeDesignator		[ phoenix::at_c<2>(label::_val) = label::_1 ]
				) 
			// Relative time (-)
			| ( byte_(0x26)				[ phoenix::at_c<0>(label::_val) = '-' ] 
				>> binDate			[ phoenix::at_c<1>(label::_val) = label::_1 ]
				>> typeDesignator		[ phoenix::at_c<2>(label::_val) = label::_1 ]
					)
	;    

	// Timezone for UTC is Z
	typeDesignator = alpha 		[ label::_val = label::_1 ]; 	


    #ifdef BOOST_SPIRIT_DEBUG
        BOOST_SPIRIT_DEBUG_NODE(typeDesignator);
        FIPA_DEBUG_RULE(bin_date_time_rule);
    #endif
    }

    qi::rule<Iterator, fipa::acl::DateTime() > bin_date_time_rule;
    grammar::DateTime<Iterator>  binDate;
    qi::rule<Iterator, char() > typeDesignator;
};



template <typename Iterator>
struct BinWord : qi::grammar<Iterator, std::string()>
{
    BinWord() : BinWord::base_type(bin_word_rule, "BinWord-bitefficient_grammar")
    {
        using qi::byte_;

	bin_word_rule = ( ( byte_(0x10) 
			>> word 			[ label::_val = label::_1 ]
			>> byte_(0x00) )
	        | byte_(0x11) >> index   		[ label::_val = extractFromCodetable(label::_1) ]
		);

        FIPA_DEBUG_RULE(bin_word_rule);
    }

    qi::rule<Iterator, std::string()> bin_word_rule;
    Word<Iterator> word;
    Index<Iterator> index;
};

template <typename Iterator>
struct BinExpression : qi::grammar<Iterator, std::string()>
{
    BinExpression() : BinExpression::base_type(bin_expression_rule, "BinExpression-bitefficient_grammar")
    {
	using qi::on_error;
	using qi::fail;
	using phoenix::construct;
	using phoenix::val;

	// Inbuild parser:
	// Encoding is ASCII here
	using encoding::char_;
	using encoding::string;
	using encoding::digit;
	using encoding::alpha;
	using qi::byte_;

	bin_expression_rule = binExpr			[ label::_val = label::_1 ] 
		      | byte_(0xFF) >> binString        [ label::_val = convertToString(label::_1) ] 
			      ;
	binExpr = binWord				[ label::_val = label::_1 ]
		| binString				[ label::_val = convertToString(label::_1) ] 
		| binNumber				[ label::_val = label::_1 ]
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
		  | ( byte_(0x72) >> digits     			 [ label::_val = label::_1 ] )
		  | ( byte_(0x73) >> digits     			 [ label::_val = label::_1 ] ) 
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
		| ( byte_(0x52) >> digits 				 [ label::_val = label::_1 ] )
		| ( byte_(0x53) >> digits 				 [ label::_val = label::_1 ] )
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


	// pass the local length variable down to the rule using parent rules variable  _r1			
	fipaString = (stringLiteral | byteLengthEncodedString(label::_r1) );

	byteLengthEncodedStringHeader = char_('#')
				      >> + digit 	[ label::_val += label::_1 ]
				      >> char_('"')
				      ;

	// Digits tell the byte encoding
	// label::_r1 is an inherited local variable from the parent rule
	byteLengthEncodedString = byteLengthEncodedStringHeader 		        [ phoenix::at_c<0>(label::_val) += label::_1 ]
				>> qi::repeat(label::_r1)[byte_]     			[ phoenix::at_c<2>(label::_val) = convertToString(label::_1) ]
				;

    #ifdef BOOST_SPIRIT_DEBUG
        BOOST_SPIRIT_DEBUG_NODE(bin_expression_rule);
	FIPA_DEBUG_RULE(byteLengthEncodedString);
    #endif
    }

    // Main rule
    qi::rule<Iterator, std::string()> bin_expression_rule;

    BinNumber<Iterator> binNumber;
    BinWord<Iterator> binWord;
    Word<Iterator> word;
    Index<Iterator> index;
    
    Digits<Iterator> digits;
    BinString<Iterator> binString;

    BinDateTime<Iterator> binDateTime;
    qi::rule<Iterator, std::string() > binExpr;
    qi::rule<Iterator, std::string(), qi::locals<boost::uint_least32_t> > exprStart;
    qi::rule<Iterator, std::string(), qi::locals<boost::uint_least32_t> > exprEnd;
    
    //qi::symbols<char, qi::rule<Iterator> > exprKeyword;
    
    qi::rule<Iterator, std::vector<unsigned char>(boost::uint32_t) > byteSeq;
    Len8<Iterator> len8;
    Len16<Iterator> len16;
    Len32<Iterator> len32;
    
    qi::rule<Iterator, fipa::acl::ByteSequence(boost::uint32_t) > fipaString;
    StringLiteral<Iterator> stringLiteral;
    qi::rule<Iterator, fipa::acl::ByteSequence() > stringLiteralTerminated;
    qi::rule<Iterator, std::string() > byteLengthEncodedStringHeader;
    qi::rule<Iterator, fipa::acl::ByteSequence(boost::uint32_t) > byteLengthEncodedString;
    ByteLengthEncodedStringTerminated<Iterator> byteLengthEncodedStringTerminated;
    CodedNumber<Iterator> codedNumber;
};

template<typename Iterator>
struct UserdefinedParameter : qi::grammar<Iterator, fipa::acl::UserDefinedParameter()>
{
    UserdefinedParameter() : UserdefinedParameter::base_type(userdefined_parameter_rule, "UserdefinedParameter-bitefficient_grammar")
    {
        userdefined_parameter_rule = qi::byte_(0x04)
                 >> binWord 	    [ phoenix::at_c<0>(label::_val) = label::_1 ]
       		 >> binExpression   [ phoenix::at_c<1>(label::_val) = label::_1 ];

        FIPA_DEBUG_RULE(userdefined_parameter_rule);
    }

    qi::rule<Iterator, fipa::acl::UserDefinedParameter() > userdefined_parameter_rule;

    BinWord<Iterator> binWord;
    BinExpression<Iterator> binExpression;
};

template<typename Iterator>
struct AgentIdentifier : qi::grammar<Iterator, fipa::acl::AgentIdentifier()>
{
    AgentIdentifier() : AgentIdentifier::base_type(agent_identifier_rule, "AgentIdentifier-bitefficient_grammar")
    {
        using qi::byte_;

        agent_identifier_rule = byte_(0x02) >> agentName 		[ phoenix::at_c<0>(label::_val) = label::_1 ]
       		 >> -addresses  	   		[ phoenix::at_c<1>(label::_val) = label::_1 ]
       		 >> -resolvers 	           		[ phoenix::at_c<2>(label::_val) = label::_1 ]
       		 >> *userDefinedParameter		[ phoenix::push_back(phoenix::at_c<3>(label::_val), label::_1) ] 
       		 >> endOfCollection;				
        
        addresses = byte_(0x02) >> urlCollection 		[ label::_val = label::_1 ];
        resolvers = byte_(0x03) >> *agent_identifier_rule       [ phoenix::push_back(label::_val, label::_1) ]
       			   >> endOfCollection;
        
        urlCollection = *url 					[ phoenix::push_back(label::_val, label::_1) ]
       		 >> endOfCollection;
        
        			;
        endOfCollection %= byte_(0x01); 

        FIPA_DEBUG_RULE(addresses);
        FIPA_DEBUG_RULE(resolvers);
	FIPA_DEBUG_RULE(agent_identifier_rule);
    }

    qi::rule<Iterator, fipa::acl::AgentIdentifier()> agent_identifier_rule;
    BinWord<Iterator> agentName;
    qi::rule<Iterator, std::vector<std::string>() > addresses;
    qi::rule<Iterator, std::vector<fipa::acl::Resolver>() > resolvers;
    UserdefinedParameter<Iterator> userDefinedParameter;
    qi::rule<Iterator, std::vector<std::string>() > urlCollection;
    BinWord<Iterator> url;
    qi::rule<Iterator> endOfCollection;
};

template <typename Iterator>
// IMPORTANT: ACLMessage with following () otherwise, compiler error
struct Message : qi::grammar<Iterator, fipa::acl::Message()>
{
      
	Message() : Message::base_type(aclCommunicativeAct, "message-bitefficient_grammar")
	{
		using phoenix::construct;
		using phoenix::val;

		using qi::lit;
		using qi::lexeme; // prevents character skipping
                using qi::byte_;
		
		
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
		
		// byte_() does only return an unused_type, so if we want to save the value, we have to assign it directly
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

		
		// Note: never do a direct assignment like
		// messageParameter = predefinedMessageParameter or you will be getting runtime errors	
		// use messageParameter = predefinedMessageParameter.alias() instead
		messageParameter = predefinedMessageParameter | userDefinedMessageParameter;

		userDefinedMessageParameter = byte_(0x00)
					    >> parameterName 		[ phoenix::at_c<0>(label::_val) = label::_1 ]
					    >> parameterValue           [ phoenix::at_c<1>(label::_val) = label::_1 ]
					    ;

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
								
		// predefinedMessageParameter uses a boost::variant
		predefinedMessageParameter = byte_(0x02) [ phoenix::at_c<0>(label::_val) = "sender" ]       >> agentIdentifier [ phoenix::at_c<1>(label::_val) = label::_1 ]    // sender
					| byte_(0x03) [ phoenix::at_c<0>(label::_val) = "receiver" ]        >> recipientExpr   [ phoenix::at_c<1>(label::_val) = label::_1 ]   // receiver 
					| byte_(0x04) [ phoenix::at_c<0>(label::_val) = "content" ]         >> msgContent      [ phoenix::at_c<1>(label::_val) = label::_1 ]   // content 
					| byte_(0x05) [ phoenix::at_c<0>(label::_val) = "reply-with" ]      >> replyWithParam  [ phoenix::at_c<1>(label::_val) = label::_1 ]   // reply-with
					| byte_(0x06) [ phoenix::at_c<0>(label::_val) = "reply-by" ]        >> replyByParam    [ phoenix::at_c<1>(label::_val) = label::_1 ]  // reply-by 
					| byte_(0x07) [ phoenix::at_c<0>(label::_val) = "in-reply-to" ]     >> inReplyToParam  [ phoenix::at_c<1>(label::_val) = label::_1 ]  // in-reply-to 
					| byte_(0x08) [ phoenix::at_c<0>(label::_val) = "reply-to" ]        >> replyToParam    [ phoenix::at_c<1>(label::_val) = label::_1 ] // reply-to   
					| byte_(0x09) [ phoenix::at_c<0>(label::_val) = "language" ]        >> language        [ phoenix::at_c<1>(label::_val) = label::_1 ] // language  
					| byte_(0x0a) [ phoenix::at_c<0>(label::_val) = "encoding" ]        >> encoding        [ phoenix::at_c<1>(label::_val) = label::_1 ] // encoding 
					| byte_(0x0b) [ phoenix::at_c<0>(label::_val) = "ontology" ]        >> ontology        [ phoenix::at_c<1>(label::_val) = label::_1 ] // ontology
					| byte_(0x0c) [ phoenix::at_c<0>(label::_val) = "protocol" ]        >> protocol        [ phoenix::at_c<1>(label::_val) = label::_1 ] // protocol
					| byte_(0x0d) [ phoenix::at_c<0>(label::_val) = "conversation-id" ] >> conversationId  [ phoenix::at_c<1>(label::_val) = label::_1 ] // conversation-id
					; 

		recipientExpr = *agentIdentifier 			[ phoenix::push_back(label::_val, label::_1) ]
				>> endOfCollection;
		
		replyToParam = recipientExpr.alias();

	// Enables the debug tree when debug mode is active
	// requires the include of stream operators as well
	// since every type will be printed to the outputstream (standard is cout)
        // see also: debug.h
	#ifdef BOOST_SPIRIT_DEBUG
	// http://boost-spirit.com/home/articles/doc-addendum/debugging/
        BOOST_SPIRIT_DEBUG_NODE(header);
        BOOST_SPIRIT_DEBUG_NODE(messageId);
        BOOST_SPIRIT_DEBUG_NODE(version);
        BOOST_SPIRIT_DEBUG_NODE(messageType);
        BOOST_SPIRIT_DEBUG_NODE(userDefinedMessageType);
        BOOST_SPIRIT_DEBUG_NODE(messageParameter);
        BOOST_SPIRIT_DEBUG_NODE(userDefinedMessageParameter);
        BOOST_SPIRIT_DEBUG_NODE(predefinedMessageParameter);
        BOOST_SPIRIT_DEBUG_NODE(predefinedMessageType);
	#endif
		
	}
	
	// Define rules as follows
	// qi::rule<Iterator, synthesized_attribute(inherited_attribute>)> r;
	// synthesized = type of output value
	// inherited = actual type the parser gives you
	//
	// if you have local variable within your rule you have to specify them here as well by using
	// qi::locals, i.e.
	// qi::rule<Iterator, fipa::acl::ByteSequence(), qi::locals<std::string> > stringLiteral;
	// here, the local variable is std::string an can be accessed within the rule as _a, _b, _c, ..
	// (check the correct namespace)
	//
	// inherited attributes: are defined withi the brackets () of the second arg of a rule
	// and can be accessed within the rule as _r1, _r2, ...
	// qi::rule<Iterator, std::vector<unsigned char>(boost::uint32_t) > byteSeq;
	// here, the rule can be called like: byteSeq(20) and will try to match as its synthesised attribute
	// a vector of 20 characters
	//
	qi::rule<Iterator, fipa::acl::Message()> aclCommunicativeAct;
	qi::rule<Iterator> message;
	qi::rule<Iterator, fipa::acl::Header()> header;
	qi::rule<Iterator, char()> messageId;
	qi::rule<Iterator, char()> version;
	qi::rule<Iterator> endOfMessage;
	qi::rule<Iterator> endOfCollection;

	qi::rule<Iterator, std::string() > messageType;
	qi::rule<Iterator, std::string() > userDefinedMessageType;
	BinWord<Iterator> messageTypeName;

	qi::rule<Iterator, fipa::acl::message::Parameter() > messageParameter;
	qi::rule<Iterator, fipa::acl::message::Parameter() > userDefinedMessageParameter;
	qi::rule<Iterator, fipa::acl::message::Parameter() > predefinedMessageParameter;
	
	BinWord<Iterator> parameterName;
	BinExpression<Iterator> parameterValue;

	qi::rule<Iterator, std::string() > predefinedMessageType;
	UserdefinedParameter<Iterator> userDefinedParameter;
	
	qi::rule<Iterator, std::vector<fipa::acl::AgentIdentifier>() > recipientExpr;

        AgentIdentifier<Iterator> agentIdentifier;

        BinExpression<Iterator> binExpression;
        BinWord<Iterator> binWord;
        BinString<Iterator> binString;
        BinString<Iterator> msgContent;

	BinExpression<Iterator> replyWithParam;
	BinDateTime<Iterator> replyByParam;
	BinExpression<Iterator> inReplyToParam;
	qi::rule<Iterator, std::vector<fipa::acl::AgentIdentifier>() > replyToParam;
	BinExpression<Iterator> language;
	BinExpression<Iterator> encoding;
	BinExpression<Iterator> ontology;
	BinWord<Iterator> protocol;
	BinExpression<Iterator> conversationId;
};

} // end namepsace bitefficient
} // end namespace acl
} // end namespace fipa

#endif // _FIPAACL_GRAMMAR_BITEFFICIENT_H_
