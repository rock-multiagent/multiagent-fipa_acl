#include "grammar_common.h"
namespace fipa {

phoenix::function<extractFromCodetableImpl> extractFromCodetable;
phoenix::function<buildStringImpl> buildString;
phoenix::function<concatStringsWithSeparatorImpl> concatStringsWithSeparator;
phoenix::function<printImpl> print;

phoenix::function<convertDigitsToHexImpl> convertDigitsToHex;
phoenix::function<convertToStringImpl> convertToString;
phoenix::function<convertToCharVectorImpl> convertToCharVector;
phoenix::function<convertToNativeShortImpl> lazy_ntohs;
phoenix::function<convertToNativeLongImpl> lazy_ntohl;
phoenix::function<createAgentIDImpl> createAgentID;
phoenix::function<convertStringToNumberImpl> convertStringToNumber;
phoenix::function<digitPaddingBytesImpl> digitPaddingBytes;

phoenix::function<convertToTimeImpl> convertToTime;
phoenix::function<convertToBaseTimeImpl> convertToBaseTime;
phoenix::function<convertToNumberTokenImpl> convertToNumberToken;


typedef std::string::const_iterator iterator_type;

namespace acl {
namespace grammar {

template struct Index<iterator_type>;
template struct Len8<iterator_type>;
template struct Len16<iterator_type>;
template struct Len32<iterator_type>;
template struct StringLiteral<iterator_type>;
template struct StringLiteralTerminated<iterator_type>;
template struct ByteLengthEncodedStringTerminated<iterator_type>;
template struct NullTerminatedString<iterator_type>;
template struct CodedNumber<iterator_type>;
template struct Digits<iterator_type>;
template struct Word<iterator_type>;
template struct DateTime<iterator_type>;

} // end namespace acl
} // end namespace bitefficient
} // end namespace fipa
