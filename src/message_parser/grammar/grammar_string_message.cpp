#include "grammar_string_message.h"

namespace fipa {

typedef std::string::const_iterator iterator_type;

namespace acl {
namespace grammar {
namespace string {

template struct AgentIdentifier<iterator_type>;
template struct AgentIdentifierSet<iterator_type>;
template struct AgentIdentifierSequence<iterator_type>;
template struct Number<iterator_type>;
template struct DateTime<iterator_type>;
template struct Expression<iterator_type>;
template struct Message<iterator_type>;

} // end namespace string
} // end namespace grammar
} // end namespace acl
} // end namespace fipa
