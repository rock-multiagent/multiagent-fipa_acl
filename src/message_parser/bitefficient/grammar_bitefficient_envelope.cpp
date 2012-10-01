#include "grammar_bitefficient_envelope.h"

namespace fipa {

typedef std::string::const_iterator iterator_type;

namespace acl {
namespace bitefficient {

template struct MessageEnvelope<iterator_type>;

} // end namespace bitefficient
} // end namespace acl
} // end namespace fipa
