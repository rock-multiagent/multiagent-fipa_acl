#ifndef FIPA_ACL_GRAMMAR_ENVELOPE_BITEFFICIENT_H
#define FIPA_ACL_GRAMMAR_ENVELOPE_BITEFFICIENT_H
#include <fipa_acl/message_generator/acl_envelope.h>
#include <fipa_acl/message_parser/bitefficient/grammar_bitefficient.h>
#include <fipa_acl/message_parser/types.h>

// BOOST_FUSION_ADAPT_CLASS has been renamed to BOOST_FUSION_ADAPT_ADT for boost version > 104500
#if BOOST_VERSION < 104500
#include <boost/fusion/include/adapt_class.hpp>
#else
#include <boost/fusion/adapted/adt/adapt_adt.hpp>
#include <boost/fusion/include/adapt_adt.hpp>
#endif

#if BOOST_VERSION < 104500
#define FIPA_ACL_FUSION_ADAPT BOOST_FUSION_ADAPT_CLASS
#else
#define FIPA_ACL_FUSION_ADAPT BOOST_FUSION_ADAPT_ADT
#endif

BOOST_FUSION_ADAPT_STRUCT(
    fipa::acl::envelope::Parameter,
    (std::string, name)
    (fipa::acl::envelope::ParameterValue, data)
);

FIPA_ACL_FUSION_ADAPT(
    fipa::acl::ACLEnvelope,
    (const fipa::acl::ACLBaseEnvelope&, const fipa::acl::ACLBaseEnvelope&, obj.getBaseEnvelope(), obj.setBaseEnvelope(val))
    (const fipa::acl::ACLBaseEnvelopeList&, const fipa::acl::ACLBaseEnvelopeList&, obj.getExtraEnvelopes(), obj.setExtraEnvelopes(val))
);

FIPA_ACL_FUSION_ADAPT(
    fipa::acl::ACLBaseEnvelope,
    (const fipa::acl::AgentIDList&, const fipa::acl::AgentIDList&, obj.getTo(), obj.setTo(val))
    (const fipa::acl::AgentID&, const fipa::acl::AgentID&, obj.getFrom(), obj.setFrom(val))
    (const fipa::acl::Comments&, const fipa::acl::Comments&, obj.getComments(), obj.setComments(val))
    (const fipa::acl::representation::Type&, const fipa::acl::representation::Type&, obj.getACLRepresentation(), obj.setACLRepresentation(val))
    (const fipa::acl::PayloadLength& , const fipa::acl::PayloadLength&, obj.getPayloadLength(), obj.setPayloadLength(val))
    (const fipa::acl::PayloadEncoding&, const fipa::acl::PayloadEncoding&, obj.getPayloadEncoding(), obj.setPayloadEncoding(val))
    (const base::Time&, const base::Time&, obj.getDate(), obj.setDate(val))
    (const fipa::acl::AgentIDList&, const fipa::acl::AgentIDList&, obj.getIntendedReceivers(), obj.setIntendedReceivers(val))
    (const fipa::acl::ReceivedObject&, const fipa::acl::ReceivedObject&, obj.getReceivedObject(), obj.setReceivedObject(val))
    (const fipa::acl::TransportBehaviour&, const fipa::acl::TransportBehaviour&, obj.getTransportBehaviour(), obj.setTransportBehaviour(val))
    ( const fipa::acl::UserdefinedParameterList&, const fipa::acl::UserdefinedParameterList&, obj.getUserdefinedParameters(), obj.setUserdefinedParameters(val))
);

FIPA_ACL_FUSION_ADAPT(
    fipa::acl::ReceivedObject,
    (const fipa::acl::URL&, const fipa::acl::URL&, obj.getBy(), obj.setBy(val))
    (const fipa::acl::URL&, const fipa::acl::URL&, obj.getFrom(), obj.setFrom(val))
    (const base::Time&, const base::Time&, obj.getDate(), obj.setDate(val))
    (const fipa::acl::ID&, const fipa::acl::ID&, obj.getId(), obj.setId(val))
    (const fipa::acl::Via&, const fipa::acl::Via&, obj.getVia(), obj.setVia(val))
    (const fipa::acl::UserdefinedParameterList&, const fipa::acl::UserdefinedParameterList&, obj.getUserdefinedParameters(), obj.setUserdefinedParameters(val))
);



namespace fipa {
// ##########################################
// Utility functions for envelope processing
// ##########################################

    struct mergeBaseEnvelopeImpl
    {
        template <typename T>
        struct result
        {
	    typedef fipa::acl::ACLBaseEnvelope type;
        };

	template <typename T>
	fipa::acl::ACLBaseEnvelope operator()(T arg0, T arg1) const
	{
	    throw std::runtime_error("mergeBaseEnvelope called with wrong arguments");
	}

	::fipa::acl::ACLBaseEnvelope operator()(const ::fipa::acl::ACLBaseEnvelope& baseEnv, const ::fipa::acl::ACLBaseEnvelope& otherEnv)
	{
	    return baseEnv.merge(otherEnv);
	}
    };

    extern phoenix::function<mergeBaseEnvelopeImpl> mergeBaseEnvelope;

namespace acl {
namespace bitefficient {

template<typename Iterator>
struct Any : qi::grammar<Iterator, std::string()>
{
    Any() : Any::base_type(any_rule, "Any-bitefficient_grammar")
    {

        any_rule = binStringNoCodetable [ label::_val = convertToString(label::_1) ];

        FIPA_DEBUG_RULE(any_rule);
    }

    qi::rule<Iterator, std::string() > any_rule;
    BinStringNoCodetable<Iterator> binStringNoCodetable;
};

template<typename Iterator>
struct Url : qi::grammar<Iterator, std::string()>
{
    Url() : Url::base_type(url_rule, "Url-bitefficient_grammar")
    {
        url_rule = nullTerminatedString [ label::_val = convertToString(label::_1)];
        FIPA_DEBUG_RULE(url_rule);
    }
    qi::rule<Iterator, std::string()> url_rule;
    NullTerminatedString<Iterator> nullTerminatedString;
};

template<typename Iterator>
struct EnvelopeAgentIdentifier : qi::grammar<Iterator, fipa::acl::AgentIdentifier()>
{
    EnvelopeAgentIdentifier() : EnvelopeAgentIdentifier::base_type(agent_identifier_rule, "EnvelopeAgentIdentifier-bitefficient_grammar")
    {
        using qi::byte_;

        agent_identifier_rule = byte_(0x02) >> agentName 		[ phoenix::at_c<0>(label::_val) = label::_1 ]
     		 >> -addresses  	   		[ phoenix::at_c<1>(label::_val) = label::_1 ]
     		 >> -resolvers 	           		[ phoenix::at_c<2>(label::_val) = label::_1 ]
     		 >> *userDefinedParameter		[ phoenix::push_back(phoenix::at_c<3>(label::_val), label::_1) ]
     		 >> endOfCollection;

        addresses = byte_(0x02) >> urlCollection 		[ label::_val = label::_1 ];
        resolvers = byte_(0x03) >> *agent_identifier_rule       [ phoenix::push_back(label::_val, label::_1) ]
    			   >> endOfCollection;

        urlCollection = *url 					[ phoenix::push_back(label::_val, label::_1) ]
    		 >> endOfCollection;

        userDefinedParameter = byte_(0x05) >> nullTerminatedString     [ phoenix::at_c<0>(label::_val) = label::_1 ]
		                   >> any                      [ phoenix::at_c<1>(label::_val) = label::_1 ]
	;

    #ifdef BOOST_SPIRIT_DEBUG
        BOOST_SPIRIT_DEBUG_NODE(agent_identifier_rule);
        BOOST_SPIRIT_DEBUG_NODE(addresses);
        BOOST_SPIRIT_DEBUG_NODE(resolvers);
        BOOST_SPIRIT_DEBUG_NODE(userDefinedParameter);
    #endif
    }

    BinWord<Iterator> binWord;
    BinStringNoCodetable<Iterator> binStringNoCodetable;

    qi::rule<Iterator, fipa::acl::AgentIdentifier()> agent_identifier_rule;
    NullTerminatedString<Iterator> agentName;

    qi::rule<Iterator, std::vector<std::string>() > addresses;
    qi::rule<Iterator, std::vector<fipa::acl::Resolver>() > resolvers;
    qi::rule<Iterator, fipa::acl::UserDefinedParameter() > userDefinedParameter;
    qi::rule<Iterator, std::vector<std::string>() > urlCollection;

    NullTerminatedString<Iterator> nullTerminatedString;
    Url<Iterator> url;
    Any<Iterator> any;
    EndOfCollection<Iterator> endOfCollection;
};

template <typename Iterator>
struct ReceivedObject : qi::grammar<Iterator, fipa::acl::ReceivedObject()>
{
    ReceivedObject() : ReceivedObject::base_type(received_object_rule, "ReceivedObject-bitefficient_grammar")
    {
        using qi::byte_;

        received_object_rule = by                [ phoenix::at_c<0>(label::_val) = label::_1 ]
            >> date                              [ phoenix::at_c<2>(label::_val) = convertToBaseTime(label::_1) ]
            >> -from                             [ phoenix::at_c<1>(label::_val) = label::_1 ]
            >> -id                               [ phoenix::at_c<3>(label::_val) = label::_1 ]
            >> -via                              [ phoenix::at_c<4>(label::_val) = label::_1 ]
            >> *userDefinedParameter             [ phoenix::push_back(phoenix::at_c<5>(label::_val), label::_1) ]
            >> endOfCollection
        ;


        from = byte_(0x02) << url                 [ label::_val = label::_1];
        id = byte_(0x03) << nullTerminatedString  [ label::_val = label::_1];
        via = byte_(0x04) << nullTerminatedString [ label::_val = label::_1];

        userDefinedParameter = qi::byte_(0x00)
                << nullTerminatedString /*keyword*/ [ phoenix::at_c<0>(label::_val) = label::_1 ]
                << nullTerminatedString             [ phoenix::at_c<1>(label::_val) = label::_1 ]
                ;
    }

    qi::rule<Iterator, fipa::acl::ReceivedObject()> received_object_rule;
    Url<Iterator> by;
    Url<Iterator> url;
    qi::rule<Iterator, fipa::acl::URL()> from;
    qi::rule<Iterator, fipa::acl::ID()> id;
    qi::rule<Iterator, fipa::acl::Via()> via;

    BinDateTime<Iterator> date;
    NullTerminatedString<Iterator> nullTerminatedString;

    qi::rule<Iterator, fipa::acl::envelope::Parameter()> userDefinedParameter;
    EndOfCollection<Iterator> endOfCollection;
};



template <typename Iterator>
struct Envelope : qi::grammar<Iterator, fipa::acl::ACLEnvelope(), qi::locals< std::vector<fipa::acl::ACLBaseEnvelope> > >
{
    Envelope() : Envelope::base_type(message_envelope_rule, "Envelope-bitefficient_grammar")
    {
        using qi::byte_;
        namespace label = qi::labels;

        message_envelope_rule = *extEnvelope [ phoenix::push_back(label::_a, label::_1) ]
            >> baseEnvelope  [ phoenix::at_c<0>(label::_val) = label::_1, phoenix::at_c<1>(label::_val) = label::_a ]
            //>> payload
        ;

        baseEnvelope = baseEnvelopeHeader [ label::_val = label::_1 ]
            >> *parameter 		  [ label::_val = mergeBaseEnvelope(label::_val, label::_1) ]
            >> endOfEnvelope
        ;

        extEnvelope = extEnvelopeHeader [ label::_val = label::_1 ]
            >> *parameter               [ label::_val = mergeBaseEnvelope(label::_val, label::_1) ]
            >> endOfEnvelope
        ;

        baseEnvelopeHeader = baseMsgId     [ phoenix::at_c<1>(label::_val) = label::_1 ]
            >> envLen                      [ phoenix::at_c<5>(label::_val) = label::_1 ]
            >> aclRepresentation           [ phoenix::at_c<4>(label::_val) = label::_1 ]
            >> date                        [ phoenix::at_c<6>(label::_val) = label::_1 ]
        ;

        extEnvelopeHeader = extMsgId       [ phoenix::at_c<1>(label::_val) = label::_1 ]
            >> envLen                      [ phoenix::at_c<5>(label::_val) = label::_1 ]
            >> receivedObject              [ phoenix::at_c<8>(label::_val) = label::_1 ]
        ;

        envLen = len16 | jumboEnvelope;

        jumboEnvelope = byte_(0x00) >> byte_(0x00)
                        >> len32           [ label::_val = label::_1 ];

        baseMsgId = byte_(0xfe)            [ label::_val = 0xfe ];
        extMsgId = byte_(0xfd)             [ label::_val = 0xfd ];

        agentIdentifierSequence = *agentIdentifier [ phoenix::push_back(label::_val, label::_1) ]
            << endOfCollection;

        parameter = predefinedParameter     [ label::_val = label::_1 ]
            | userdefinedParameter          [ phoenix::push_back(phoenix::at_c<10>(label::_val), label::_1) ]
        ;

	userDefinedMessageParameter = byte_(0x00)
				    >> parameterName 		[ phoenix::at_c<0>(label::_val) = label::_1 ]
				    >> parameterValue           [ phoenix::at_c<1>(label::_val) = label::_1 ]
	;

        predefinedParameter = byte_(0x02) >> agentIdentifierSequence [ phoenix::at_c<0>(label::_val) = label::_1 ]
            | byte_(0x03) >> agentIdentifier        [ phoenix::at_c<1>(label::_val) = label::_1 ]
            | byte_(0x04) >> aclRepresentation      [ phoenix::at_c<2>(label::_val) = label::_1 ]
            | byte_(0x05) >> comments               [ phoenix::at_c<3>(label::_val) = label::_1 ]
            | byte_(0x06) >> payloadLength          [ phoenix::at_c<4>(label::_val) = label::_1 ]
            | byte_(0x07) >> payloadEncoding        [ phoenix::at_c<5>(label::_val) = label::_1 ]
            // date is not set here
            | byte_(0x08) >> intendedReceiver       [ phoenix::at_c<7>(label::_val) = label::_1 ]
            | byte_(0x09) >> receivedObject         [ phoenix::at_c<8>(label::_val) = label::_1 ]
            | byte_(0x10) >> transportBehaviour     [ phoenix::at_c<9>(label::_val) = label::_1 ]
        ;

        aclRepresentation = userDefinedACLRepresentation
            | byte_(0x10) [ label::_val = representation::TypeTxt[representation::BITEFFICIENT] ] // [FIPA00069]
            | byte_(0x11) [ label::_val = representation::TypeTxt[representation::STRING] ]// [FIPA00070]
            | byte_(0x12) [ label::_val = representation::TypeTxt[representation::XML] ] // [FIPA00071]
        ;

        intendedReceiver = agentIdentifierSequence.alias();

        userDefinedACLRepresentation = byte_(0x00)
            >> nullTerminatedString [ label::_val = label::_1 ];
    }

    qi::rule<Iterator, fipa::acl::envelope::Parameter() > messageParameter;
    qi::rule<Iterator, fipa::acl::envelope::Parameter() > userdefinedParameter;
    qi::rule<Iterator, fipa::acl::envelope::Parameter() > predefinedParameter;

    qi::rule<Iterator, fipa::acl::ACLEnvelope(), std::vector<fipa::acl::ACLBaseEnvelope> > message_envelope_rule;
    qi::rule<Iterator, fipa::acl::ACLBaseEnvelope> extEnvelope;
    qi::rule<Iterator, fipa::acl::ACLBaseEnvelope> extEnvelopeHeader;
    qi::rule<Iterator, fipa::acl::ACLBaseEnvelope> baseEnvelopeHeader;
    qi::rule<Iterator, fipa::acl::ACLBaseEnvelope> baseEnvelope;
    qi::rule<Iterator, fipa::acl::envelope::Parameter> parameter;
    EndOfCollection<Iterator> endOfEnvelope;
    EndOfCollection<Iterator> endOfCollection;

    ReceivedObject<Iterator> receivedObject;
    BinDateTime<Iterator> date;

    qi::rule<Iterator, fipa::acl::envelope::Parameter> userDefinedMessageParameter;
    qi::rule<Iterator, std::string> parameterName;
    qi::rule<Iterator, std::string> parameterValue;

    NullTerminatedString<Iterator> comments;
    BinNumber<Iterator> payloadLength;
    NullTerminatedString<Iterator> payloadEncoding;
    Any<Iterator> transportBehaviour;

    qi::rule<Iterator, boost::int_least32_t> envLen;
    Len16<Iterator> len16;
    Len32<Iterator> len32;
    Len32<Iterator> jumboEnvelope;

    qi::rule<Iterator, std::string()> aclRepresentation;
    qi::rule<Iterator, std::string()> userDefinedACLRepresentation;

    NullTerminatedString<Iterator> nullTerminatedString;
    NullTerminatedString<Iterator> keyword;

    qi::rule<Iterator> baseMsgId;
    qi::rule<Iterator> extMsgId;
    BinExpression<Iterator> binExpression;
    qi::rule<Iterator> url;

    EnvelopeAgentIdentifier<Iterator> agentIdentifier;
    qi::rule<Iterator, std::vector<fipa::acl::AgentIdentifier()> > agentIdentifierSequence;
    qi::rule<Iterator, std::vector<fipa::acl::AgentIdentifier()> > intendedReceiver;
    qi::rule<Iterator, fipa::acl::UserDefinedParameter() > userDefinedParameter;
};

} // end namespace bitefficient
} // end namespace acl
} // end namespace fipa
#endif // FIPA_ACL_GRAMMAR_ENVELOPE_BITEFFICIENT_H

