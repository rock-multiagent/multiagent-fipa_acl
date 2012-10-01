#ifndef FIPA_ACL_GRAMMAR_ENVELOPE_BITEFFICIENT_H
#define FIPA_ACL_GRAMMAR_ENVELOPE_BITEFFICIENT_H
#include <fipa_acl/message_generator/acl_message_envelope.h>
#include <fipa_acl/message_parser/bitefficient/grammar_bitefficient.h>

namespace fipa {
namespace acl {
namespace bitefficient {

//BOOST_FUSION_ADAPT_ADT(
//        fipa::acl::ACLMessageEvelope,
//        (const AgentIDList&, const AgentIDList&, obj.getTo(), obj.setTo(val))
//        (const AgentID&, const AgentID&, obj.getFrom(), obj.setFrom(val))
//        (const Comments&, const Comments&, obj.getComments(), obj.setComments(val))
//        (const ACLRepresentation&, const ACLRepresentation&, obj.getACLRepresentation(), obj.setACLRepresentation(val))
//        (const PayloadLength&, const PayloadLength&, obj.getPayloadLength(), obj.setPayloadLength(val))
//        (const PayloadEncoding&, const PayloadEncoding&, obj.getPayloadEncoding(), obj.setPayloadEncoding())
//        (const base::Time&, const base::Time&, obj.getDate(), obj.setDate())
//        (const AgentIDList&, const AgentIDList&, obj.getIntendedReceiver(), obj.setIntendedReceiver(val))
//        (const ReceivedObject&, const ReceivedObject&, obj.getReceivedObject(), obj.setReceivedObject(val))
//        (const TransportBehaviour&, const TransportBehaviour&, obj.getTransportBehaviour(), obj.setTransportBehaviour(val))
//);


template <typename Iterator>
struct MessageEnvelope : qi::grammar<Iterator, fipa::acl::ACLMessageEnvelope()>
{
    MessageEnvelope() : MessageEnvelope::base_type(message_envelope_rule, "MessageEnvelope-bitefficient_grammar")
    {
        using qi::byte_;
        namespace label = qi::labels;

        message_envelope_rule = byte_ [label::_val = fipa::acl::ACLMessageEnvelope() ]; //#endOfCollection;
//        message_envelope_rule = *extEnvelope
//            >> baseEnvelope 
//            >> payload
//        ;
//
//        baseEnvelope = baseEnvelopeHeader
//            >> *parameter
//            >> endOfEnvelope
//        ;
//
//        extEnvelope = extEnvelopeHeader
//            >> *parameter
//            >> endOfEnvelope
//        ;
//
//        baseEnvelopeHeader = baseMsgId
//            >> envLen
//            >> aclRepresentation
//            >> date
//        ;
//
//        extEnvelopeHeader = extMsgId
//            >> envLen
//            >> receivedObject
//        ;
//
//        envLen = len16 >> jumboEnvelope;
//
//        baseMsgId = byte_(0xfe);
//        extMsgId = byte_(0xfd);
//
//        endOfCollection %= byte_(0x01);
//        envOfEnvelope %= endOfCollection.alias();
//
//        parameter = predefinedParameter 
//            | userdefinedParameter;
//
//        predefinedParameter = byte_(0x02) >> agentIdentifierSequence [ phoenix::at_c<0>(label::_val) = label::_1 ]
//            | byte_(0x03) >> agentIdentifier        [ phoenix::at_c<1>(label::_val) = label::_1 ]
//            | byte_(0x04) >> aclRepresentation      [ phoenix::at_c<2>(label::_val) = label::_1 ]
//            | byte_(0x05) >> comments               [ phoenix::at_c<3>(label::_val) = label::_1 ]
//            | byte_(0x06) >> payloadLength          [ phoenix::at_c<4>(label::_val) = label::_1 ]
//            | byte_(0x07) >> payloadEncoding        [ phoenix::at_c<5>(label::_val) = label::_1 ]
//            // date is not set here
//            | byte_(0x08) >> intendedReceiver       [ phoenix::at_c<7>(label::_val) = label::_1 ]
//            | byte_(0x09) >> receivedObject         [ phoenix::at_c<8>(label::_val) = label::_1 ]
//            | byte_(0x10) >> transportBehaviour     [ phoenix::at_c<9>(label::_val) = label::_1 ]
//        ;
//        aclRepresentation = userDefinedACLRepresentation
//            | byte_(0x10) // bitefficient [FIPA00069]
//            | byte_(0x11) // string       [FIPA00070]
//            | byte_(0x12) // xml          [FIPA00071]
//        ;
//
//        comments = nullTerminatedString;
//        payloadLength = binNumber;
//        payloadEncoding = nullTerminatedString;
//        intendedReceiver = agentIdentifierSequence.alias();
//        transportBehaviour = any;
//        userDefinedACLRepresentation = byte_(0x00)
//            >> nullTerminatedString;
//
//        receivedObject = by
//            >> date
//            >> -from
//            >> -id
//            >> -via
//            >> *userDefinedParameter
//            >> endOfCollection
//        ; 
//
//        by = url.alias();
//        from = byte_(0x02) << url;
//        id = byte_(0x03) << nullTerminatedString;
//        via = byte_(0x04) << nullTerminatedString;
//
//        binNumber = digits.alias();
//	digits = +codedNumber [ label::_val += label::_1];
//
//        nullTerminatedString = binString >> byte_(0x00);
//        userdefinedParameter = byte_(0x00) >> keyword >> nullTerminatedString;
//        keyword = nullTerminatedString.alias();
//        any = byte_(0x14) 
//            >> nullTerminatedString
//            | byteLengthEncoded;
//
    }

    qi::rule<Iterator, fipa::acl::ACLMessageEnvelope()> message_envelope_rule;
//    qi::rule<Iterator> endOfMessage;
//    qi::rule<Iterator> endOfCollection;
//    BinDateTime<Iterator> date;

};

} // end namespace bitefficient
} // end namespace acl
} // end namespace fipa
#endif // FIPA_ACL_GRAMMAR_ENVELOPE_BITEFFICIENT_H

