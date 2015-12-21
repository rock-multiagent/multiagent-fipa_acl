#ifndef FIPA_ACL_GRAMMAR_COMMON_H
#define FIPA_ACL_GRAMMAR_COMMON_H

//#define BOOST_SPIRIT_DEBUG

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>

#ifndef BOOST_SPIRIT_USE_PHOENIX_V3
#include <boost/spirit/home/support/context.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#else
#include <boost/phoenix/phoenix.hpp>
#include <boost/phoenix/fusion/at.hpp>
#include <boost/fusion/adapted/std_pair.hpp>
#endif

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <arpa/inet.h>
#include <ctime>

#include <base/logging.h>

#include <fipa_acl/message_generator/acl_message.h>

#include <fipa_acl/message_parser/types.h>
#ifdef BOOST_SPIRIT_DEBUG
// include stream operators
#include <fipa_acl/message_parser/debug.h>

#define FIPA_DEBUG_RULE(X) BOOST_SPIRIT_DEBUG_NODE(X); qi::on_error<qi::fail> ( X, std::cout << phoenix::val("Error: expecting ") << label::_4 /* what failed? */ << phoenix::val(" here: \"") << phoenix::construct<std::string>(label::_3,label::_2) << phoenix::val("\"") << std::endl);
#else
#define FIPA_DEBUG_RULE(X) 
#endif

// BOOST_FUSION_ADAPT_CLASS has been renamed to BOOST_FUSION_ADAPT_ADT for boost version > 104500
#if BOOST_VERSION < 104500
#include <boost/fusion/include/adapt_class.hpp>
#else
#include <boost/fusion/adapted/adt/adapt_adt.hpp>
#include <boost/fusion/include/adapt_adt.hpp>
#endif

#if BOOST_VERSION < 104500
#define FIPA_ACL_FUSION_ADAPT BOOST_FUSION_ADAPT_CLASS
#else
#define FIPA_ACL_FUSION_ADAPT BOOST_FUSION_ADAPT_ADT
#endif

FIPA_ACL_FUSION_ADAPT(
    fipa::acl::AgentID,
    (const std::string&, const std::string&, obj.getName(), obj.setName(val))
    (const std::vector<std::string>&, const std::vector<std::string>&, obj.getAddresses(), obj.setAddresses(val))
    (const std::vector<fipa::acl::AgentID>&, const std::vector<fipa::acl::AgentID>&, obj.getResolvers(), obj.setResolvers(val))
    (const std::vector<fipa::acl::UserdefParam>&, const std::vector<fipa::acl::UserdefParam>&, obj.getUserdefParams(), obj.setUserdefParams(val))
    );

FIPA_ACL_FUSION_ADAPT(
    fipa::acl::UserdefParam,
    (const std::string&, const std::string&, obj.getName(), obj.setName(val))
    (const std::string&, const std::string&, obj.getValue(), obj.setValue(val))
    );

FIPA_ACL_FUSION_ADAPT(
        fipa::acl::ACLMessage, 
        /* 0 */  (std::string, std::string, obj.getPerformative(), obj.setPerformative(val))
        /* 1 */  (fipa::acl::AgentID, fipa::acl::AgentID, obj.getSender(), obj.setSender(val))
        /* 2 */  (fipa::acl::AgentIDList, fipa::acl::AgentIDList, obj.getAllReceivers(), obj.setAllReceivers(val))
        /* 3 */  (std::string, std::string, obj.getContent(), obj.setContent(val))
        /* 4 */  (std::string, std::string, obj.getReplyWith(), obj.setReplyWith(val))
        /* 5 */  (base::Time, base::Time, obj.getReplyBy(), obj.setReplyBy(val))
        /* 6 */  (std::string, std::string, obj.getInReplyTo(), obj.setInReplyTo(val))
        /* 7 */  (fipa::acl::AgentIDList, fipa::acl::AgentIDList, obj.getAllReplyTo(), obj.setAllReplyTo(val))
        /* 8 */  (std::string, std::string, obj.getLanguage(), obj.setLanguage(val))
        /* 9 */  (std::string, std::string, obj.getEncoding(), obj.setEncoding(val))
        /*10 */  (std::string, std::string, obj.getOntology(), obj.setOntology(val))
        /*11 */  (std::string, std::string, obj.getProtocol(), obj.setProtocol(val))
        /*12 */  (std::string, std::string, obj.getConversationID(), obj.setConversationID(val))
        /*13 */  (void, void, /*no getter used*/, obj.addUserdefParam(val))
);

namespace fusion = boost::fusion;
namespace phoenix = boost::phoenix;
namespace spirit = boost::spirit;
namespace qi = boost::spirit::qi;

// Using ascii encoding
namespace encoding = boost::spirit::ascii;
//namespace encoding = boost::spirit::standard;

BOOST_FUSION_ADAPT_STRUCT(
	fipa::acl::ByteSequence,
	(std::string, encoding)
	(fipa::acl::LengthValue, length)
	(fipa::acl::ByteString, bytes)
)

// We only need additional access to the millisecond element
BOOST_FUSION_ADAPT_STRUCT(
	fipa::acl::Time,
	(int, tm_msec)
)

BOOST_FUSION_ADAPT_STRUCT(
	fipa::acl::DateTime,
	(char, relative)
	(fipa::acl::Time, dateTime)
	(char, timezone)
)

namespace fipa {

//#####################################################
// Utility functions
//#####################################################
	// In order to use functions as semantic actions
	// lazy evaluation is required	
        struct extractFromCodetableImpl
	{
		typedef std::string result_type;

		template <typename T>
		struct result
		{
			typedef result_type type;
		};

		template <typename T>
		result_type operator()(T arg) const
		{
			if(typeid(T) == typeid(unsigned short))
			{
				//unsigned short index;
                                //// codetable index
				//index = arg;

                                // TODO: search codetable and extract data
			}

			throw std::runtime_error("Codetable currently unsupported");
		}
	
	};

	extern phoenix::function<extractFromCodetableImpl> extractFromCodetable;

	/** Concatenation of two string as 'lazy' implementation for boost spirit*/
	struct buildStringImpl
	{
		typedef std::string result_type;

		template <typename T, typename U, typename V>
		struct result
		{
			typedef result_type type;
		};

		template <typename T, typename U, typename V>
		result_type operator()(T arg0, U arg1, V arg2) const
		{
			arg0 += arg1;
			arg0 += arg2;
			return arg0;
		}


	};
	extern phoenix::function<buildStringImpl> buildString;

	/** Concatenation of two strings as 'lazy' implementation for boost spirit using a dedicated separation character*/
	struct concatStringsWithSeparatorImpl
	{
		typedef std::string result_type;

		template <typename T, typename U, typename V>
		struct result
		{
			typedef result_type type;
		};

		template <typename T, typename U, typename V>
		result_type operator()(T arg0, U arg1, V arg2) const
		{
                    if(arg0.empty())
                    {
                        return arg1;
                    } else {
			arg0 += arg2;
			arg0 += arg1;
			return arg0;
                    }
		}
	};
	extern phoenix::function<concatStringsWithSeparatorImpl> concatStringsWithSeparator;

	/** Debug output */
	struct printImpl
	{
	        typedef void result_type;

		template <typename T, typename U>
		struct result
		{
			typedef result_type type;
		};

		template <typename T, typename U>
		result_type operator()(T arg0, U arg1) const
		{
			printf("%s %s\n", arg0, arg1.c_str());
		}

	};

	extern phoenix::function<printImpl> print;

        struct digitPaddingBytesImpl
        {
             typedef uint32_t result_type;

             template <typename T>
             struct result
             {
                 typedef result_type type;
             };
             
             template <typename T>
             result_type operator()(T digit /* std::string */) const
             {
                   char lastbyte = digit[0];
                   // odd number of characters
                   // check if last byte is properly padded
                   if( (lastbyte & 0x0F) == 0)
                   {
                       // properly padded
                       LOG_DEBUG("CodedNumber properly padded byte detected: %x", lastbyte);
                       return 0;
                   } else {
                       LOG_DEBUG("CodedNumber with unpadded byte detected: %x", lastbyte);
                       return 1;
                   }
             }
        };

	extern phoenix::function<digitPaddingBytesImpl> digitPaddingBytes;

	/** Convert byte to number according to the FIPA definition */
	struct convertToNumberTokenImpl
	{
		typedef std::string result_type;

		template <typename T>
		struct result
		{
			typedef result_type type;
		};
		
		template <typename T> 
		result_type operator()(T arg) const
		{

			// Each bytes contains two numbers: 
			// one in the highbytes and one in the lowerbytes
			char highbytes = arg;
			char lowerbytes = arg;

			// Shift to lowerbytes so that we can use the convert 
			// function here as well
			highbytes >>= 4;

			std::string tmp = convert(highbytes);
			tmp += convert(lowerbytes);

			return std::string(tmp);
		}

		std::string convert(char lowerbyte) const
		{
			// make sure high order bytes are set to 0	
			lowerbyte &= 0x0F;	
			
			// There will be more efficient ways to do that, but for clarity
			switch(lowerbyte)
			{
				case 0x00: return "";
				case 0x01: return "0";
				case 0x02: return "1";
				case 0x03: return "2";
				case 0x04: return "3";
				case 0x05: return "4";
				case 0x06: return "5";
				case 0x07: return "6";
				case 0x08: return "7";
				case 0x09: return "8";
				case 0x0a: return "9";
				case 0x0c: return "+";
				case 0x0d: return "E";
				case 0x0e: return "-";
				case 0x0f: return ".";
				default: return "";
			}
		}
	};

	extern phoenix::function<convertToNumberTokenImpl> convertToNumberToken;

	/**
	* Convert a string that represents an integer to a string that represent an hexadecimal number
	*/
	struct convertDigitsToHexImpl
	{
		typedef std::string result_type;

		template <typename T>
		struct result
		{
			typedef result_type type;
		};

		template <typename T> 
		result_type operator()(T arg) const
		{
			return "";
		}
		
		result_type operator()(std::string arg) const
		{
			unsigned int hexNumber = atoi(arg.c_str());
			
			char buffer[512];
			snprintf(buffer,512,"%#x",hexNumber);

			return std::string(buffer);
		}
	};

	extern phoenix::function<convertDigitsToHexImpl> convertDigitsToHex;

	struct convertToNativeShortImpl
	{
		typedef uint16_t result_type;

		template <typename T>
		struct result
		{
			typedef result_type type;
		};

		// for std::vector<char>
		template <typename T>
		result_type operator()(T arg) const
		{
			return ntohs(arg);
		}
	};

	extern phoenix::function<convertToNativeShortImpl> lazy_ntohs;

	struct convertToNativeLongImpl
	{
		typedef uint32_t result_type;

		template <typename T>
		struct result
		{
			typedef result_type type;
		};

		// for std::vector<char>
		template <typename T>
		result_type operator()(T arg) const
		{
			return ntohl(arg); 
		}
	};

	extern phoenix::function<convertToNativeLongImpl> lazy_ntohl;

        
	/** Convert types to strings */
	struct convertToStringImpl
	{
		typedef std::string result_type;

		template <typename T>
		struct result
		{
			typedef result_type type;
		};

		// for std::vector<char>
		template <typename T>
		result_type operator()(T arg) const
		{
			return std::string(arg.begin(), arg.end());
		}

		result_type operator()(fipa::acl::ByteSequence arg) const
		{
			// TODO: (optional) perform some encoding stuff here
			// using arg.encoding
			return arg.toRawDataString();
		}


	};

	extern phoenix::function<convertToStringImpl> convertToString;

	/** Convert types to vector of characters */
	struct convertToCharVectorImpl
	{
		typedef std::vector<unsigned char> result_type;

		template <typename T>
		struct result
		{
			typedef result_type type;
		};

		template<typename T>
		result_type operator()(T arg) const
		{
			int length = arg.size();
			std::vector<unsigned char> tmp;

			for(int i = 0; i < length; i++)
			{
				tmp.push_back((unsigned char) arg[i]);	
	
			}
			return tmp; 
		}

	};
	
	extern phoenix::function<convertToCharVectorImpl> convertToCharVector;

    struct createAgentIDImpl
    {
        typedef fipa::acl::AgentID result_type;

        template <typename T>
        struct result
        {
            typedef result_type type;
        };

        template<typename T>
        fipa::acl::AgentID operator()(T arg) const
        {
            return fipa::acl::AgentID(arg);
        }

    };

	extern phoenix::function<createAgentIDImpl> createAgentID;

    struct convertStringToNumberImpl
    {
        typedef boost::uint32_t result_type;
        template <typename T>
        struct result
        {
            typedef result_type type;
        };

        template<typename T>
        boost::uint32_t operator()(T arg) const
        {
            std::string number(arg.begin(),arg.begin() + strlen(arg.c_str()));
            try {
                return boost::lexical_cast<boost::uint32_t>(number);
            } catch(const std::bad_cast& e)
            {
                LOG_ERROR("ConvertString failed: '%s'", arg.c_str());
                LOG_ERROR("String_size: '%d'", arg.size());
                LOG_ERROR("String_length: '%d'", strlen(arg.c_str()));
                std::string msg = "ConvertStringToNumber failed for '" + number + "' " + std::string(e.what());
                throw std::runtime_error(msg);
            }
        }
    };

	extern phoenix::function<convertStringToNumberImpl> convertStringToNumber;


	/** Convert String to Time */
	struct convertToTimeImpl
	{
		typedef fipa::acl::Time result_type;

		template <typename T, typename U>
		struct result
		{
			typedef result_type type;
		};

		result_type operator()(const std::string& arg, const std::string& msecs) const
		{
			fipa::acl::Time convertedTime;
#ifdef BOOST_SPIRIT_DEBUG
			printf("convertToTimeImpl: %s:%s\n", arg.c_str(), msecs.c_str());
#endif
			// TODO: windows portage
			strptime(arg.c_str(),"%Y-%m-%dT%H:%M:%S",&convertedTime);
			convertedTime.tm_msec = atoi(msecs.c_str());
			
			return convertedTime;
		}
	};

	extern phoenix::function<convertToTimeImpl> convertToTime;

	/** Convert Time to base::Time */
	struct convertToBaseTimeImpl
	{
		typedef base::Time result_type;

		template <typename T>
		struct result
		{
			typedef result_type type;
		};

		template <typename T>
		result_type operator()(T arg) const
		{
			return arg.toTime();
		}

		result_type operator()(fipa::acl::DateTime arg) const
		{
			return arg.toTime();
		}

                result_type operator()(std::string arg) const
                {
                    std::string time = arg;
                    if ( arg.data()[0] == '-' || arg.data()[0] == '+')
                    {
                        time.erase(0,1);
                    }
                    std::string format = "%Y%m%dT%H%M%S";

                    size_t end = time.find_last_of("0123456789", time.size() -1);
                    if( end < time.size() - 1)
                    {
                        // the type designator is present
                        format += "%Z";
                    }

                    // Shift millisecond so that base::Time understands
                    // the format
                    std::string milliseconds = time.substr(end-2,3);
                    time.erase(end-2,3);
                    time += ":" + milliseconds;

                    return base::Time::fromString(time, base::Time::Milliseconds, format);

                }
	};
	
	extern phoenix::function<convertToBaseTimeImpl> convertToBaseTime;

        

namespace acl {
namespace grammar {
// To avoid namespace clashes with boost::bind
namespace label = qi::labels;

template<typename Iterator>
struct Index : qi::grammar<Iterator, uint_least16_t()>
{
    Index() : Index::base_type(index_rule, "Index-bitefficient_grammar")
    {
	// Index is a pointer to code table entry and its size (in bits) depends on the code table size. 
	// If the code table size is 256 entries, the size of the index is one byte;
	//  otherwise its size is two bytes (represented in network byte order).
	index_rule = qi::word     [ label::_val = lazy_ntohs(label::_1)] 
                   | qi::byte_    [ label::_val = label::_1 ]
                   ;

        FIPA_DEBUG_RULE(index_rule);
    }

    qi::rule<Iterator, uint_least16_t() > index_rule;
};

template<typename Iterator>
struct Len8 : qi::grammar<Iterator, boost::uint_least8_t()>
{
    Len8(): Len8::base_type(len8_rule, "Len8-bitefficient_grammar")
    {
	len8_rule = qi::byte_;

        FIPA_DEBUG_RULE(len8_rule);
    }

    qi::rule<Iterator, boost::uint_least8_t() > len8_rule;
};

template<typename Iterator>
struct Len16 : qi::grammar<Iterator, boost::uint_least16_t()>
{
    Len16(): Len16::base_type(len16_rule, "Len16-bitefficient_grammar")
    {
        // boost::uint_least16_t
	len16_rule = qi::word [ label::_val = lazy_ntohs(label::_1) ];

	FIPA_DEBUG_RULE(len16_rule);
    }

    qi::rule<Iterator, boost::uint_least16_t() > len16_rule;
};

template<typename Iterator>
struct Len32 : qi::grammar<Iterator, boost::uint_least32_t()>
{
    Len32(): Len32::base_type(len32_rule, "Len32-bitefficient_grammar")
    {
        // boost::uint_least32_t
	len32_rule = qi::dword [ label::_val = lazy_ntohl(label::_1) ];

	FIPA_DEBUG_RULE(len32_rule);
    }

    qi::rule<Iterator, boost::uint_least32_t() > len32_rule;
};

template <typename Iterator>
struct StringLiteral : qi::grammar<Iterator, fipa::acl::ByteSequence(), qi::locals<std::string> >
{
    StringLiteral() : StringLiteral::base_type(string_literal_rule, "StringLiteral-bitefficient_grammar")
    {
        using qi::byte_;
	using encoding::char_;
	using encoding::digit;

	// Order of statement is relevant here, since the character \ needs to be matched
	// first -- matching is greedy with char_ otherwise
	string_literal_rule = ( char_('"')
		       >> * (( char_("\\") >> char_('"') ) 	[  label::_a += "\"" ]
			  | (byte_ - char_('"') ) 		[  label::_a += label::_1 ]
		          )
		       >> char_('"')
			)					[ phoenix::at_c<2>(label::_val) = label::_a ]
			;

	FIPA_DEBUG_RULE(string_literal_rule);
    }

    qi::rule<Iterator, fipa::acl::ByteSequence(), qi::locals<std::string> > string_literal_rule;
};

template<typename Iterator>
struct StringLiteralTerminated : qi::grammar<Iterator, fipa::acl::ByteSequence() >
{
    StringLiteralTerminated() : StringLiteralTerminated::base_type(string_literal_terminated_rule, "StringLiteralTerminated-bitefficient_grammar")
    {
	string_literal_terminated_rule = stringLiteral 		[ label::_val = label::_1 ]
				>> qi::byte_(0x00)
				;

	FIPA_DEBUG_RULE(string_literal_terminated_rule);
    }
 
    qi::rule<Iterator, fipa::acl::ByteSequence()> string_literal_terminated_rule;
    StringLiteral<Iterator> stringLiteral;
};

template<typename Iterator>
struct ByteLengthEncodedString : qi::grammar<Iterator, fipa::acl::ByteSequence(), qi::locals<uint32_t> >
{
    ByteLengthEncodedString() : ByteLengthEncodedString::base_type(byte_length_encoded_string_rule, "ByteLengthEncodedString-common_grammar")
    {
        using qi::byte_;
        using encoding::char_;
	using encoding::digit;

	byte_length_encoded_string_rule = byteLengthEncodedStringHeader 	[ label::_a = convertStringToNumber(label::_1) ]
					>> qi::repeat(label::_a)[byte_]			[ phoenix::at_c<2>(label::_val) = convertToCharVector(label::_1) ]
					;

	byteLengthEncodedStringHeader = char_('#')
				      >> + digit 	[ label::_val += label::_1 ]
				      >> char_('"')
				      ;

	FIPA_DEBUG_RULE(byte_length_encoded_string_rule);
    }

    qi::rule<Iterator, std::string() > byteLengthEncodedStringHeader;
    qi::rule<Iterator, fipa::acl::ByteSequence(), qi::locals<uint32_t> > byte_length_encoded_string_rule;
};

template<typename Iterator>
struct ByteLengthEncodedStringTerminated : qi::grammar<Iterator, fipa::acl::ByteSequence(), qi::locals<std::string> >
{
    ByteLengthEncodedStringTerminated() : ByteLengthEncodedStringTerminated::base_type(byte_length_encoded_string_terminated_rule, "ByteLengthEncodedStringTerminated-bitefficient_grammar")
    {
        using qi::byte_;
        using encoding::char_;
	using encoding::digit;

	byte_length_encoded_string_terminated_rule = ( byteLengthEncodedStringHeader 	[ phoenix::at_c<0>(label::_val) += label::_1 ]
					>> * (byte_ - byte_(0x00))			[ label::_a += label::_1 ]
					>> byte_(0x00)
					) 						[ phoenix::at_c<2>(label::_val) = label::_a ] 
					;

	byteLengthEncodedStringHeader = char_('#')
				      >> + digit 	[ label::_val += label::_1 ]
				      >> char_('"')
				      ;

	FIPA_DEBUG_RULE(byte_length_encoded_string_terminated_rule);
    }

    qi::rule<Iterator, std::string() > byteLengthEncodedStringHeader;
    qi::rule<Iterator, fipa::acl::ByteSequence(), qi::locals<std::string> > byte_length_encoded_string_terminated_rule;
};

template<typename Iterator>
struct NullTerminatedString : qi::grammar<Iterator, fipa::acl::ByteSequence()>
{
    NullTerminatedString() : NullTerminatedString::base_type(null_terminated_string_rule, "NullTerminatedString-bitefficient_grammar")
    {

        null_terminated_string_rule = stringLiteralTerminated
                | byteLengthEncodedStringTerminated
		;

	FIPA_DEBUG_RULE(null_terminated_string_rule);
    }

    qi::rule<Iterator, fipa::acl::ByteSequence()> null_terminated_string_rule;
    StringLiteralTerminated<Iterator> stringLiteralTerminated;
    ByteLengthEncodedStringTerminated<Iterator> byteLengthEncodedStringTerminated;
};

template <typename Iterator>
struct Word : qi::grammar<Iterator, std::string()>
{
    Word() : Word::base_type(word_rule, "Word-bitefficient_grammar")
    {
        using encoding::char_;

	word_rule = (char_ - wordExceptionsStart )  	[ label::_val += label::_1 ]
		 >> *(char_ - wordExceptionsGeneral)    [ label::_val += label::_1 ]
		;

	wordExceptionsStart %= wordExceptionsGeneral
                        | char_('#') 
			| char_('0','9')
			| char_('-')
			| char_('@')
			;
	wordExceptionsGeneral %= char_(0x00,0x20) 
			| char_('(')
                        | char_(')')
			;
    #ifdef BOOST_SPIRIT_DEBUG
        BOOST_SPIRIT_DEBUG_NODE(wordExceptionsStart);
        BOOST_SPIRIT_DEBUG_NODE(wordExceptionsGeneral);

        FIPA_DEBUG_RULE(word_rule);
    #endif
    }

    qi::rule<Iterator, std::string()> word_rule;
    qi::rule<Iterator> wordExceptionsStart;
    qi::rule<Iterator> wordExceptionsGeneral;
};

template<typename Iterator>
struct Digits : qi::grammar<Iterator, std::string(), qi::locals<boost::uint_least8_t> >
{
    Digits() : Digits::base_type(digits_rule, "Digits-bitefficient_grammar")
    {
        using encoding::char_;
        digits_rule = lower_bits_padding                [ label::_val += convertToNumberToken(label::_1) ]
                   | (+(qi::byte_ - end_padding_marker) [ label::_val += convertToNumberToken(label::_1) ]
                      >> end_padding_marker             [ label::_val += convertToNumberToken(label::_1) ]
                     )
                   ;

        // padding bytes should only apply if the last codedNumber does not have 00 in the lowerbyte
        end_padding_marker = char_(0x00) | lower_bits_padding;
        lower_bits_padding = qi::byte_     [ label::_val = label::_1 ]
                           >> qi::eps( !(label::_val & 0x0f) )
                           ;

	FIPA_DEBUG_RULE(digits_rule);
        FIPA_DEBUG_RULE(end_padding_marker);
        FIPA_DEBUG_RULE(lower_bits_padding);

    }

    qi::rule<Iterator, std::string(), qi::locals<boost::uint_least8_t> > digits_rule;
    qi::rule<Iterator, boost::uint_least8_t()> end_padding_marker;
    qi::rule<Iterator, boost::uint_least8_t()> lower_bits_padding;
};


template<typename Iterator>
struct CodedNumber : qi::grammar<Iterator, std::string()>
{
    CodedNumber() : CodedNumber::base_type(coded_number_rule, "CodedNumber-bitefficient_grammar")
    {
        // two numbers in one byte - padding 00 if coding only one number
        coded_number_rule = (qi::byte_ - qi::byte_(0x00))[ label::_val = convertToNumberToken(label::_1)]
                          ;

	FIPA_DEBUG_RULE(coded_number_rule);
    }

    qi::rule<Iterator, std::string()> coded_number_rule;
};

template <typename Iterator>
struct DateTime : qi::grammar<Iterator, fipa::acl::Time(), qi::locals<std::string> >
{
    DateTime() : DateTime::base_type(date_time_rule, "DateTime-bitefficient_grammar")
    {

        // Construct/Fill Time()
        // First read the standard input for struct tm
        // then fill the extended millisecond field
        
        // Fixme: label::_a = string, label::_a += "-" will return only label::_a as "-"
        // Currently using a workaround with 'buildString'
        date_time_rule = ((        ( year	[ label::_a = buildString(label::_1,"-","") ]) //, label::_a = label::_1, label::_a += "-" ])//, print("yearinBinDate:", label::_1) ]*/ )
        		>> ( month	[ label::_a = buildString(label::_a, label::_1,"-") ]) 
        		>> ( day	[ label::_a = buildString(label::_a, label::_1,"T") ])
        		>> ( hour       [ label::_a = buildString(label::_a, label::_1,":") ])
        		>> ( minute	[ label::_a = buildString(label::_a, label::_1,":") ])
        		>> ( second	[ label::_a = buildString(label::_a, label::_1,"")  ]) 
        	   ) 		
        	>> ( millisecond  	[ label::_val = convertToTime(label::_a, label::_1) ])
        	)
        	 ;
        
        year = ( codedNumber  	[ label::_val = label::_1] )
               >> ( codedNumber [ label::_val += label::_1 ])
               ;
        // same format as year
        millisecond = year.alias();

    #ifdef BOOST_SPIRIT_DEBUG
        BOOST_SPIRIT_DEBUG_NODE(year);
        BOOST_SPIRIT_DEBUG_NODE(millisecond);
    #endif // BOOST_SPIRIT_DEBUG

	FIPA_DEBUG_RULE(date_time_rule);
    }

    CodedNumber<Iterator> codedNumber;

    qi::rule<Iterator, fipa::acl::Time(), qi::locals<std::string> > date_time_rule;
    qi::rule<Iterator, std::string() > year;
    CodedNumber<Iterator> month;
    CodedNumber<Iterator> day;
    CodedNumber<Iterator> hour;
    CodedNumber<Iterator> minute;
    CodedNumber<Iterator> second;
    qi::rule<Iterator, std::string() > millisecond;

};

template<typename Iterator>
struct String : qi::grammar<Iterator, std::string()>
{
    String() : String::base_type(string_rule, "String-common_grammar")
    {
	string_rule = stringLiteral     [ label::_val = convertToString(label::_1) ]
            | byteLengthEncodedString   [ label::_val = convertToString(label::_1) ] 
        ;

	FIPA_DEBUG_RULE(string_rule);
    }

    StringLiteral<Iterator> stringLiteral;
    ByteLengthEncodedString<Iterator> byteLengthEncodedString; 
    qi::rule<Iterator, std::string()> string_rule; 
};

} // end namespace grammar
} // end namespace acl
} // end namespace fipa



#endif // FIPA_ACL_GRAMMAR_COMMON_H
