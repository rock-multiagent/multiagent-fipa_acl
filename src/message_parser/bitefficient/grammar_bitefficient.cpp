#include "grammar_bitefficient.h"
/**
    \file grammar_bitefficient.cpp
    \brief delcares instances needed for the grammar
    \author Mircea Cretu Stancu
*/
namespace fipa {

phoenix::function<extractFromCodetableImpl> extractFromCodetable;
phoenix::function<buildStringImpl> buildString;
phoenix::function<printImpl> print;
phoenix::function<convertToTimeImpl> convertToTime;
phoenix::function<convertToBaseTimeImpl> convertToBaseTime;
phoenix::function<convertToNumberTokenImpl> convertToNumberToken;
phoenix::function<convertDigitsToHexImpl> convertDigitsToHex;
phoenix::function<convertToStringImpl> convertToString;
phoenix::function<convertToCharVectorImpl> convertToCharVector;
phoenix::function<convertToNativeShortImpl> lazy_ntohs;
phoenix::function<convertToNativeLongImpl> lazy_ntohl;

typedef std::string::const_iterator iterator_type;

namespace acl {
namespace bitefficient {

template struct Index<iterator_type>;
template struct Len8<iterator_type>;
template struct Len16<iterator_type>;
template struct Len32<iterator_type>;
template struct CodedNumber<iterator_type>;
template struct Digits<iterator_type>;
template struct BinNumber<iterator_type>;
template struct DateTime<iterator_type>;
template struct BinDateTime<iterator_type>;
template struct StringLiteral<iterator_type>;
template struct StringLiteralTerminated<iterator_type>;
template struct NullTerminatedString<iterator_type>;
template struct ByteLengthEncodedStringTerminated<iterator_type>;
template struct BinStringNoCodetable<iterator_type>;
template struct BinStringCodetable<iterator_type>;
template struct BinString<iterator_type>;
template struct Word<iterator_type>;
template struct BinWord<iterator_type>;
template struct BinExpression<iterator_type>;
template struct AgentIdentifier<iterator_type>;
template struct Message<iterator_type>;

} // end namespace bitefficient
} // end namespace acl
} // end namespace fipa
