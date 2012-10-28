#include "grammar_bitefficient_envelope.h"

namespace fipa {

typedef std::string::const_iterator iterator_type;

phoenix::function<mergeBaseEnvelopeImpl> mergeEnvelope;

namespace acl {
namespace bitefficient {

template struct Url<iterator_type>;
template struct Any<iterator_type>;
//template struct MessageEnvelope<iterator_type>;

} // end namespace bitefficient
} // end namespace acl
} // end namespace fipa
