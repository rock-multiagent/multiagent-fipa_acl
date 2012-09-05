#ifndef FIPA_ACL_MESSAGEGENERATOR_TYPES
#define FIPA_ACL_MESSAGEGENERATOR_TYPES

#include <string>

namespace fipa {
namespace acl {
 
    /**
     * Typedefinition for generic fiedls of a fipa message
     */
    typedef std::string Ontology;
    typedef std::string Language;
    typedef std::string Content;
    typedef std::string Protocol;
    typedef std::string Performative;
    typedef std::string ConversationID;
    typedef std::string Encoding;
    typedef std::string ReplyBy;
    typedef std::string InReplyTo;
    typedef std::string ReplyWith;
    
} // end of acl
} // end of fipa

#endif
