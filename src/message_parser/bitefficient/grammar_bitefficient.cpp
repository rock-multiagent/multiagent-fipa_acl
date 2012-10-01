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
phoenix::function<convertToNumberTokenImpl> convertToNumberToken;
phoenix::function<convertDigitsToHexImpl> convertDigitsToHex;
phoenix::function<convertToStringImpl> convertToString;
phoenix::function<convertToCharVectorImpl> convertToCharVector;
phoenix::function<convertToNativeShortImpl> lazy_ntohs;
phoenix::function<convertToNativeLongImpl> lazy_ntohl;

typedef std::string::const_iterator iterator_type;

namespace acl {
namespace bitefficient {

template struct CodedNumber<iterator_type>;
template struct DateTime<iterator_type>;
template struct BinDateTime<iterator_type>;
template struct Index<iterator_type>;
template struct Word<iterator_type>;
template struct BinWord<iterator_type>;
template struct BinExpression<iterator_type>;
template struct AgentIdentifier<iterator_type>;
template struct Message<iterator_type>;

} // end namespace bitefficient
} // end namespace acl
} // end namespace fipa
