#ifndef FIPA_ACL_GRAMMAR_STRING_MESSAGE_H
#define FIPA_ACL_GRAMMAR_STRING_MESSAGE_H

#include <fipa_acl/message_parser/grammar/grammar_common.h>
#include <fipa_acl/message_generator/message_format.h>

namespace fipa {
namespace acl {
namespace string {

//template<typename Iterator>
//struct Message : qi::grammar<Iterator, fipa::acl::ACLMessage()>
//{
//
//    Message() : Message::base_type(aclCommunicativeAct, "message-string_grammar")
//    {
//	using phoenix::construct;
//	using phoenix::val;
//
//        using MessageField;
//
//	// To avoid namespace clashes with boost::bind
//	namespace label = qi::labels;
//
//        actCommunicativeAct = "("
//            //>> MessageFieldTxt[SENDER] >> sender
//            //>> MessageFieldTxt[RECEIVER] >> receivers
//            >> MessageFieldTxt[CONTENT] >> content         [ phoenix::at_c<2>(label::_val) = label::_1 ]
//            >> ")";
//
//        content =  
//    }
//
//    NullTerminatedString() content;
//    qi::rule<Iterator, fipa::acl::ACLMessage()> aclCommunicativeAct;
//
//}

} // end namespace string
} // end namespace acl
} // end namespace fipa


#endif //FIPA_ACL_GRAMMAR_STRING_MESSAGE_H
