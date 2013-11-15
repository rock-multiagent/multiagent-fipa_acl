#include "grammar_bitefficient.h"
/**
    \file grammar_bitefficient.cpp
    \brief declares instances needed for the grammar
    \author Mircea Cretu Stancu
*/
namespace fipa {

typedef std::string::const_iterator iterator_type;

namespace acl {
namespace bitefficient {

template struct EndOfCollection<iterator_type>;
template struct BinNumber<iterator_type>;
template struct BinDateTime<iterator_type>;
template struct BinStringNoCodetable<iterator_type>;
template struct BinStringCodetable<iterator_type>;
template struct BinString<iterator_type>;
template struct BinWord<iterator_type>;
template struct BinExpression<iterator_type>;
template struct UserdefinedParameter<iterator_type>;
template struct AgentIdentifier<iterator_type>;
template struct Message<iterator_type>;

} // end namespace bitefficient
} // end namespace acl
} // end namespace fipa
