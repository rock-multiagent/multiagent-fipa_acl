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

}
