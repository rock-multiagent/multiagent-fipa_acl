#include "message_format.h"

namespace fipa {
namespace acl {
    
MessageFormat::MessageFormat()
    : mUseCodeTables(false)
    , mUpdateCodeTables(true)
    , mVersion("1.3")
    , mResolverDepth(1)
{
}

}//end of acl namespace
}// end of fipa namespace

