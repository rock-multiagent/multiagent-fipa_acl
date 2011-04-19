#ifndef __FIPAACL_MESSAGEPARSER_BYTESEQUENCE_H_
#define __FIPAACL_MESSAGEPARSER_BYTESEQUENCE_H_

#include <stdio.h>
#include <string>
#include <vector>

#include <boost/cstdint.hpp>

namespace fipa { namespace acl {

/**
* \brief Definition of a length - internally used by the parser 
*/
typedef boost::variant<boost::uint_least8_t,boost::uint_least16_t, boost::uint_least32_t> LengthValue;

/**
* \brief Definition of a ByteString
*/
typedef boost::variant<std::string, std::vector<unsigned char> > ByteString;

/**
* \class ByteStringPrinter
* \brief Printer using the visitor pattern for the ByteString type
*/
class ByteStringPrinter : public boost::static_visitor<std::string>
{
	
	/** Character encoding to be used */
	std::string encoding;
public: 
	/**
         * Constructor using a given encoding, userdefined by a string representation
        */

	ByteStringPrinter(std::string enc) : encoding(enc) {}
	
	/**
	* Build a string for the given type
	* \param s One of the types for the variant that this printer is implemented for
	*/
	std::string operator()(std::string s) const
	{
		return s;
	}
	
	/**
	* Build a string from a raw byte sequence, i.e. the following output format will be used, 
        * embedding encoding information
        *  HEX(dword)[0a af 02 10 ... 01]
	* \param vector One of the types for the variant that this printer is implemented for
	*/
	std::string operator()(std::vector<unsigned char> vector) const
	{
		int length = vector.size();
		std::string tmp("HEX(");
		tmp += encoding;
		tmp +=")[";

		for(int i = 0; i < length; i++)
		{
			char currentChar; 
			sprintf(&currentChar, "%x", vector[i]);
			tmp += tmp;
			if( i%2 == 0 )
				tmp += " ";
		}

		tmp += "]";

		return std::string(tmp);
	}
};

/**
 * \class ByteSequence 
 * \brief Representation of a byte sequence. We also embed information about the encoding
 * \details whereas the encoding is defined by a natural number, i.e. 
 * postprocessing has to be performed to convert it to string
 */
struct ByteSequence
{
	std::string encoding;
	fipa::acl::LengthValue length;
	fipa::acl::ByteString bytes;

	/**
         * Convert a bytesequence to a string representation
         * This will be a defined format: see ByteStringPrinter for that
	 * \return A string containing the byte sequence
         */
	std::string toString()
	{
		std::string tmp;
		// Since bytes is a variant we apply the visitor pattern here
		tmp += boost::apply_visitor( ByteStringPrinter(encoding), bytes);
		return tmp;
	}
};



}} // namespace fipa::acl

#endif
