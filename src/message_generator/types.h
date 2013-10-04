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

    namespace representation {
        // Naming of string with appendix, to avoid clashes where keyword 'string' 
        // cannot be handled properly, e.g. CORBA idl
        enum Type { UNKNOWN, BITEFFICIENT, STRING_REP, XML, END_MARKER };

        const std::string TypeTxt[] = { "unknown", "fipa.acl.rep.bitefficient.std", "fipa.acl.rep.string.std", "fipa.acl.rep.xml.std"};
    }

} // end of acl
} // end of fipa
#endif // FIPA_ACL_MESSAGEGENERATOR_TYPES
