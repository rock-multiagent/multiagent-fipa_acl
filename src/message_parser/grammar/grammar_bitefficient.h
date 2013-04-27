#ifndef _FIPAACL_GRAMMAR_BITEFFICIENT_H_
#define _FIPAACL_GRAMMAR_BITEFFICIENT_H_
/**
 *
 * \file grammar_bitefficient.h
 * \author Thomas Roehr, thomas.roehr@dfki.de
 * \brief This grammar represents the bitefficient message specification
 * of the Foundation for Intelligent Physical Agents (FIPA at http://www.fipa.org)
 */
//#define BOOST_SPIRIT_DEBUG

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
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <arpa/inet.h>
#include <ctime>

#include <base/logging.h>

#include <fipa_acl/message_parser/types.h>
#include <fipa_acl/message_generator/agent_id.h>

#ifdef BOOST_SPIRIT_DEBUG
// include stream operators
#include <fipa_acl/message_parser/debug.h>

#define FIPA_DEBUG_RULE(X) BOOST_SPIRIT_DEBUG_NODE(X); qi::on_error<qi::fail> ( X, std::cout << phoenix::val("Error: expecting ") << label::_4 /* what failed? */ << phoenix::val(" here: \"") << phoenix::construct<std::string>(label::_3,label::_2) << phoenix::val("\"") << std::endl);
#else
#define FIPA_DEBUG_RULE(X) 
#endif

namespace fusion = boost::fusion;
namespace phoenix = boost::phoenix;
namespace spirit = boost::spirit;
namespace qi = boost::spirit::qi;

// Using ascii encoding
namespace encoding = boost::spirit::ascii;
//namespace encoding = boost::spirit::standard;



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
		template <typename T, typename U, typename V>
		struct result
		{
			typedef std::string type;
		};

		template <typename T, typename U, typename V>
		std::string operator()(T arg0, U arg1, V arg2) const
		{
			arg0 += arg1;
			arg0 += arg2;
			return arg0;
		}


	};
	extern phoenix::function<buildStringImpl> buildString;
	
	/** Debug output */
	struct printImpl
	{
		template <typename T, typename U>
		struct result
		{
			typedef void type;
		};

		template <typename T, typename U>
		void operator()(T arg0, U arg1) const
		{
			printf("%s %s\n", arg0, arg1.c_str());
		}

	};

	extern phoenix::function<printImpl> print;

	/** Convert String to Time */
	struct convertToTimeImpl
	{
		template <typename T, typename U>
		struct result
		{
			typedef fipa::acl::Time type;
		};

		template <typename T, typename U>
		fipa::acl::Time operator()(T arg0, U arg1) const
		{
			return  fipa::acl::Time();
		}

		fipa::acl::Time operator()(std::string arg, std::string msecs) const
		{
			fipa::acl::Time	convertedTime;
#ifdef BOOST_SPIRIT_DEBUG
			printf("convertToTimeImpl: %s:%s\n", arg.c_str(), msecs.c_str());
#endif
			// TODO: windows portage
			strptime(arg.c_str(),"%Y-%m-%dT%H:%M:%S",&convertedTime);
			convertedTime.tm_msec = atoi(msecs.c_str());
			
			return fipa::acl::Time(convertedTime);
		}
	};

	extern phoenix::function<convertToTimeImpl> convertToTime;

	/** Convert Time to base::Time */
	struct convertToBaseTimeImpl
	{
		template <typename T>
		struct result
		{
			typedef base::Time type;
		};

		template <typename T>
		base::Time operator()(T arg) const
		{
			return arg.toTime();
		}

		base::Time operator()(fipa::acl::DateTime arg) const
		{
			return arg.toTime();
		}
	};
	
	extern phoenix::function<convertToBaseTimeImpl> convertToBaseTime;

        struct digitPaddingBytesImpl
        {
             template <typename T>
             struct result
             {
                 typedef uint32_t type;
             };
             
             template <typename T>
             uint32_t operator()(T digit /* std::string */) const
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
		template <typename T>
		struct result
		{
			typedef std::string type;
		};
		
		template <typename T> 
		std::string operator()(T arg) const
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
		template <typename T>
		struct result
		{
			typedef std::string type;
		};

		template <typename T> 
		std::string operator()(T arg) const
		{
			return "";
		}
		
		std::string operator()(std::string arg) const
		{
			unsigned int hexNumber = atoi(arg.c_str());
			
			char buffer[512];
			sprintf(buffer,"%#x",hexNumber);

			return std::string(buffer);
		}
	};

	extern phoenix::function<convertDigitsToHexImpl> convertDigitsToHex;


	struct convertToNativeShortImpl
	{
		template <typename T>
		struct result
		{
			typedef uint16_t type;
		};

		// for std::vector<char>
		template <typename T>
		uint16_t operator()(T arg) const
		{
			return ntohs(arg);
		}
	};

	extern phoenix::function<convertToNativeShortImpl> lazy_ntohs;

	struct convertToNativeLongImpl
	{
		template <typename T>
		struct result
		{
			typedef uint32_t type;
		};

		// for std::vector<char>
		template <typename T>
		uint32_t operator()(T arg) const
		{
			return ntohl(arg); 
		}
	};

	extern phoenix::function<convertToNativeLongImpl> lazy_ntohl;

        
	/** Convert types to strings */
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
			// TODO: (optional) perform some encoding stuff here
			// using arg.encoding
			return arg.toRawDataString();
		}


	};

	extern phoenix::function<convertToStringImpl> convertToString;

	/** Convert types to vector of characters */
	struct convertToCharVectorImpl
	{
		template <typename T>
		struct result
		{
			typedef std::vector<unsigned char> type;
		};

		template<typename T>
		std::vector<unsigned char> operator()(T arg) const
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
        template <typename T>
        struct result
        {
            typedef fipa::acl::AgentID type;
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
        template <typename T>
        struct result
        {
            typedef boost::uint32_t type;
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

template<typename Iterator>
struct Digits : qi::grammar<Iterator, std::string(), qi::locals<std::string> >
{
    Digits() : Digits::base_type(digits_rule, "Digits-bitefficient_grammar")
    {
        // store the last byte to validate the padding
	digits_rule = *((qi::byte_ - qi::byte_(0x00)) [ label::_val += convertToNumberToken(label::_1)]) [ label::_a = label::_1]
                    >> qi::repeat(digitPaddingBytes(label::_a))[qi::byte_(0x00)]
        // padding bytes should only apply if the last codedNumber does not have 00 in the lowerbyte

	FIPA_DEBUG_RULE(digits_rule);
    }

    qi::rule<Iterator, std::string(), qi::locals<std::string> > digits_rule; 
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
    DateTime<Iterator>  binDate;
    qi::rule<Iterator, char() > typeDesignator;
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
