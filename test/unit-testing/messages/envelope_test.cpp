#include <boost/test/auto_unit_test.hpp>
#include <fipa_acl/fipa_acl.h>
#include <fipa_acl/message_generator/envelope_generator.h>
#include <fipa_acl/message_generator/format/bitefficient_format.h>
#include <fipa_acl/message_generator/format/bitefficient_envelope_format.h>
#include <fipa_acl/message_generator/format/xml_format.h>
#include <fipa_acl/message_generator/format/xml_envelope_format.h>
#include <fipa_acl/message_parser/envelope_parser.h>
#include <fipa_acl/message_parser/grammar/grammar_bitefficient_envelope.h>
#include "test_utils.h"

    BOOST_AUTO_TEST_SUITE(fipa_envelope_test_suite)

    BOOST_AUTO_TEST_CASE(envelope_interface_test)
    {
        using namespace fipa::acl;

        ACLBaseEnvelope envelope;
        base::Time now = base::Time::now();
        envelope.setDate(now);
        BOOST_REQUIRE_MESSAGE(envelope.contains(envelope::DATE), "Envelope contains data");
        BOOST_REQUIRE_MESSAGE(now == envelope.getDate(), "Envelope.getDate()");
        BOOST_REQUIRE_MESSAGE(envelope.contains(envelope::DATE), "Envelope contains(envelope::DATE)");
    }

    BOOST_AUTO_TEST_CASE(envelope_stamping_test)
    {
        using namespace fipa::acl;

        std::vector<std::string> hops;
        hops.push_back("mts-0");
        hops.push_back("mts-1");
        hops.push_back("mts-2");

        AgentID sender("test-sender");
        AgentID receiver("test-receiver");
        ACLMessage msg(ACLMessage::REQUEST);
        msg.setContent("test-content");

        msg.setSender(sender);
        msg.addReceiver(receiver);
        ACLEnvelope envelope(msg, representation::BITEFFICIENT);

        std::vector<std::string>::const_iterator it = hops.begin();
        for(; it != hops.end(); ++it)
        {
            AgentID agent(*it);
            envelope.stamp(agent);
        }

        AgentIDList deliveryPath = envelope.getDeliveryPath();
        for(size_t i = 0; i < hops.size(); ++i)
        {
            BOOST_REQUIRE_MESSAGE(deliveryPath[i].getName() == hops[i], "Hop is " << hops[i]);
        }

        ACLBaseEnvelopeList envelopes = envelope.getExtraEnvelopes();
        BOOST_REQUIRE(envelopes.size() == hops.size());
        ACLBaseEnvelopeList::const_iterator eit = envelopes.begin();
        for(; eit != envelopes.end(); ++eit)
        {
            ReceivedObject ro = eit->getReceivedObject();
            BOOST_REQUIRE_MESSAGE(!ro.getId().empty(), "ReceivedObject id: " << ro.getId());
        }
    }

    BOOST_AUTO_TEST_CASE(grammar_test)
    {
        using namespace fipa::acl;
        std::string storage;
        {
            storage = char(0x14) + fipa::acl::BitefficientFormat::getNullTerminatedString("ANY");
            std::string any = testGrammar<fipa::acl::bitefficient::Any, std::string>(storage);
        }

        {
            storage = fipa::acl::BitefficientFormat::getNullTerminatedString("http://www.rock-robotics.org");
            std::string url = testGrammar<fipa::acl::bitefficient::Url, std::string>(storage);
        }

        {
            AgentID inAgentID("test-agent");
            storage = fipa::acl::BitefficientFormat::getAgentID(inAgentID);
            fipa::acl::AgentID outAgentID = testGrammar<fipa::acl::bitefficient::EnvelopeAgentIdentifier, fipa::acl::AgentID>(storage, true, "Envelope: AgentID -- simple");

        }

        {
            AgentID inAgentID("test-agent");
            AgentID resolver0("test-resolver0");
            AgentID resolver1("test-resolver1");
            inAgentID.addResolver(resolver0);
            inAgentID.addResolver(resolver1);
            storage = fipa::acl::BitefficientFormat::getAgentID(inAgentID);

            //std::string::const_iterator cit = storage.begin();
            //for(; cit != storage.end(); ++cit)
            //{
            //    printf("%c --> %x\n", *cit, *cit);
            //}
            fipa::acl::AgentID outAgentID = testGrammar<fipa::acl::bitefficient::EnvelopeAgentIdentifier, fipa::acl::AgentID>(storage, true, "Envelope: AgentID -- resolvers");

        }
        {
            AgentID inAgentID("test-agent");
            AgentID resolver0("test-resolver0");
            AgentID resolver1("test-resolver1");
            inAgentID.addAddress("test-address0");
            inAgentID.addAddress("test-address1");
            inAgentID.addResolver(resolver0);
            inAgentID.addResolver(resolver1);
            inAgentID.addUserdefParam(UserdefParam("test-userparam0","setting0"));
            inAgentID.addUserdefParam(UserdefParam("test-userparam1","setting1"));
            storage = fipa::acl::BitefficientFormat::getAgentID(inAgentID);

            //std::string::const_iterator cit = storage.begin();
            //for(; cit != storage.end(); ++cit)
            //{
            //    printf("%c --> %x\n", *cit, *cit);
            //}
            fipa::acl::AgentID outAgentID = testGrammar<fipa::acl::bitefficient::EnvelopeAgentIdentifier, fipa::acl::AgentID>(storage, true, "Envelope: AgentID -- resolvers, addresses, user params");

        }

        {
            ReceivedObject inReceivedObject;
            storage = fipa::acl::BitefficientFormat::getReceivedObject(inReceivedObject);
            fipa::acl::ReceivedObject outReceivedObject = testGrammar<fipa::acl::bitefficient::ReceivedObject, fipa::acl::ReceivedObject>(storage, true, "Envelope: receivedObject");
        }

        { 
            ACLMessage msg;
            ACLEnvelope envelope(msg, representation::BITEFFICIENT);
            ACLBaseEnvelope inBaseEnvelope;
            // BEGIN Base envelope
            base::Time now = base::Time::now();
            TransportBehaviour inTransportBehaviour = "CUSTOM";
            Comments inComments = "COMMENTS";
            std::vector<AgentID> inIntendedReceivers;
            AgentID inSender("sender");
            AgentID inIntendedReceiver("intended-receiver");
            inIntendedReceivers.push_back(inIntendedReceiver);
            ReceivedObject inReceivedObject;

            inBaseEnvelope.setDate(now);
            inBaseEnvelope.setTransportBehaviour(inTransportBehaviour);
            inBaseEnvelope.setComments(inComments);
            // TODO: where to set what kind of representation!?!?
            // leaving the following out will lead to problems
            inBaseEnvelope.setACLRepresentation(representation::BITEFFICIENT);
            inBaseEnvelope.setTransportBehaviour(inTransportBehaviour);
            inBaseEnvelope.setFrom(inSender);
            inBaseEnvelope.setIntendedReceivers(inIntendedReceivers);
            inBaseEnvelope.setReceivedObject(inReceivedObject);

            // END Base envelope
            envelope.setBaseEnvelope(inBaseEnvelope);
            // BEGIN Extra envelope
            ACLBaseEnvelope inExtraEnvelope;
            AgentID inExtraTo("extra-receiver");
            AgentIDList inExtraTos;
            inExtraTos.push_back(inExtraTo);
            inExtraEnvelope.setTo(inExtraTos);
            // END Extra envelope
            envelope.addExtraEnvelope(inExtraEnvelope);

        storage = EnvelopeGenerator::create(envelope, representation::BITEFFICIENT);

        fipa::acl::EnvelopeParser ep;
        ACLEnvelope decodedEnvelope;
        BOOST_REQUIRE(ep.parseData(storage, decodedEnvelope, representation::BITEFFICIENT));

        ACLBaseEnvelope outBaseEnvelope = decodedEnvelope.getBaseEnvelope();
        BOOST_REQUIRE_MESSAGE(outBaseEnvelope.getFrom() == inSender, "From '" << outBaseEnvelope.getFrom().getName() << "' vs '" << inSender.getName() << "'");
        BOOST_REQUIRE_MESSAGE(outBaseEnvelope.getDate().toString(base::Time::Milliseconds) == inBaseEnvelope.getDate().toString(base::Time::Milliseconds), "Date '" << outBaseEnvelope.getDate().toString(base::Time::Milliseconds) << "' vs '" << inBaseEnvelope.getDate().toString(base::Time::Milliseconds));
        BOOST_REQUIRE_MESSAGE(outBaseEnvelope.getTransportBehaviour() == inTransportBehaviour, "TransportBehaviour '" << outBaseEnvelope.getTransportBehaviour() << "' vs '" << inTransportBehaviour);
        BOOST_REQUIRE(outBaseEnvelope.getACLRepresentation() == representation::BITEFFICIENT);
        BOOST_REQUIRE_MESSAGE(outBaseEnvelope.getComments() == inComments, "Comments '" << outBaseEnvelope.getComments() << "' vs '" << inComments);
        BOOST_REQUIRE_MESSAGE(outBaseEnvelope.getIntendedReceivers() == inIntendedReceivers, "IntendedReceivers '" << outBaseEnvelope.getIntendedReceivers()[0].getName() << "' vs '" << inIntendedReceiver.getName());
        BOOST_REQUIRE(outBaseEnvelope.getReceivedObject() == inReceivedObject);

        std::vector<ACLBaseEnvelope> outExtra = decodedEnvelope.getExtraEnvelopes();
        BOOST_REQUIRE_MESSAGE(outExtra.size() == 1, "'" << outExtra.size() << "' extra envelopes available, expected 1");
        BOOST_REQUIRE_MESSAGE(outExtra[0].getTo() == inExtraTos, "Extra to: '" << outExtra[0].getTo()[0].getName() << "' vs '" << inExtraTo.getName() << "'");

        ACLBaseEnvelope outFlattenedEnvelope = decodedEnvelope.flattened();
        BOOST_REQUIRE(outFlattenedEnvelope.getTo() == inExtraTos);
    }
}

BOOST_AUTO_TEST_CASE(envelope_bitefficient_test)
{
    using namespace fipa::acl;

    ACLMessage msg("inform");
    AgentID origin("proxy");
    AgentID receiver("receiver");

    AgentID resolver0("resolver0");
    AgentID resolver1("resolver1");

    receiver.addResolver(resolver0);
    receiver.addResolver(resolver1);

    msg.setSender(origin);
    msg.addReceiver(receiver);
    msg.addReplyTo(origin);
    msg.setPerformative(ACLMessage::REQUEST);
    msg.setProtocol(std::string("test-protocol"));
    msg.setLanguage(std::string("test language"));
    msg.setEncoding(std::string("test encoding"));
    msg.setOntology(std::string("test ontology"));
    msg.setReplyWith(std::string("test reply_with"));
    base::Time time = base::Time::fromString("20101223-12:00:37", base::Time::Seconds);
    msg.setReplyBy(time);
    msg.setConversationID(std::string("test conversationID"));
    msg.setContent("test-content");

    representation::Type representationType = representation::BITEFFICIENT;
    ACLEnvelope envelope(msg, representation::BITEFFICIENT);

    std::string encodedEnvelope = EnvelopeGenerator::create(envelope, representationType);
    BOOST_TEST_MESSAGE("Encoded envelope " << encodedEnvelope);
    //std::string::const_iterator cit = encodedEnvelope.begin();
    //for(; cit != encodedEnvelope.end(); ++cit)
    //{
    //    printf("%c --> %x\n", *cit, *cit);
    //}

    EnvelopeParser ep;
    ACLEnvelope decodedEnvelope;
    BOOST_REQUIRE_MESSAGE( ep.parseData(encodedEnvelope, decodedEnvelope, representationType), "Decoding ACLEnvelope " << encodedEnvelope);
    // Check embedding acl message in envelope
    ACLMessage outMsg = decodedEnvelope.getACLMessage();

    BOOST_REQUIRE_MESSAGE( msg.getPerformative() == outMsg.getPerformative(), "ACL Message in envelope performative: '" << outMsg.getPerformative() << "' vs '" << msg.getPerformative() << "'");
    BOOST_REQUIRE_MESSAGE( msg.getContent() == outMsg.getContent(), "ACL Message in envelope content: '" << outMsg.getContent() << "' vs '" << msg.getContent() << "'");
    BOOST_CHECK_MESSAGE( msg.getReplyBy() == outMsg.getReplyBy(), "ACL Message in envelope reply by (Fails when BOOST_SPIRIT_DEBUG is defined): '" << outMsg.getReplyBy().toString() << "' vs '" << msg.getReplyBy().toString() << "'");
    BOOST_CHECK_MESSAGE( msg.getLanguage() == outMsg.getLanguage(), "ACL Message in envelope language: '" << outMsg.getLanguage() << "' vs '" << msg.getLanguage() << "'");
    BOOST_CHECK_MESSAGE( msg.getEncoding() == outMsg.getEncoding(), "ACL Message in envelope encoding: '" << outMsg.getEncoding() << "' vs '" << msg.getEncoding() << "'");
    BOOST_CHECK_MESSAGE( msg.getOntology() == outMsg.getOntology(), "ACL Message in envelope ontology: '" << outMsg.getOntology() << "' vs '" << msg.getOntology() << "'");
    BOOST_REQUIRE_MESSAGE( msg == outMsg, "Full message comparisons '" << outMsg.toString() << "' vs '" << msg.toString() << "'");

}

BOOST_AUTO_TEST_CASE(envelope_xml_test)
{
    using namespace fipa::acl;

    // TODO include every single field
    ACLMessage msg("inform");
    AgentID origin("proxy");
    AgentID receiver("receiver");

    AgentID resolver0("resolver0");
    AgentID resolver1("resolver1");

    receiver.addResolver(resolver0);
    receiver.addResolver(resolver1);

    msg.setSender(origin);
    msg.addReceiver(receiver);
    msg.addReplyTo(origin);
    msg.setPerformative(ACLMessage::REQUEST);
    msg.setProtocol(std::string("test-protocol"));
    msg.setLanguage(std::string("test language"));
    msg.setEncoding(std::string("test encoding"));
    msg.setOntology(std::string("test ontology"));
    msg.setReplyWith(std::string("test reply_with"));
    base::Time time = base::Time::fromString("20101223-12:00:37", base::Time::Seconds);
    msg.setReplyBy(time);
    msg.setConversationID(std::string("test conversationID"));
    msg.setContent("test-content");

    ACLEnvelope envelope(msg, representation::XML);
    
    representation::Type envRepresentationType = representation::XML;
    std::string encodedEnvelope = EnvelopeGenerator::create(envelope, envRepresentationType);
    std::cout << "XML Encoded envelope:" << std::endl << encodedEnvelope << std::endl;
    
    // TODO ...
}

BOOST_AUTO_TEST_SUITE_END()
