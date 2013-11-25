#ifndef FIPA_ACL_H
#define FIPA_ACL_H
#include <fipa_acl/bitefficient_message.h>
#include <fipa_acl/message_generator/acl_envelope.h>
#include <fipa_acl/message_generator/serialized_letter.h>

/*! \mainpage FIPA ACL Library
 * This class provides a representation of a message conforming to the FIPA specifications hosted at http://www.fipa.org
 *
 * This library implements the fipa acl message in two different encodings: bitefficient and string encoding.
 * In addition it provides the implementation of the bitefficient message envelope.
 * No implementation exists for a string representation of the message envelope, since according to
 * http://www.fipa.org/specs/fipa00073/DC00073D.html this standard has been deprecated.
 *
 * Next the standard implementation of message and envelope types, this library provides a statemachine
 * to monitor a message flow based on performatives.
 *
 * \section Examples
 * The following list typical application examples when using this library.
 *
 * \subsection A Creating messages
 * \verbatim
 #include <fipa_acl/fipa_acl.hpp>

 using namespace fipa::acl;

 ACLMessage message;
 message.setPerformative(ACLMessage::REQUEST_WHEN);
 message.setContent("actual payload data");
 message.setSender(AgentID("sender-agent"));
 message.addReceiver(AgentID("receiver-agent-0"));
 message.addReceiver(AgentID("receiver-agent-1"));
 message.addReplyTo(AgentID("sender-agent"));
 message.setInReplyTo("in-reply-to-nothing");
 message.setReplyBy(base::Time::now());
 message.setConversationID("identifier-of-the-conservation");
 message.setProtocol("protocol-of-message-flow");
 message.setLanguage("a-content-language");
 message.setEncoding("encoding-of-content");
 message.setOntology("an-ontology");
 \endverbatim
 *
 * \subsection A Generate messages in a specific representation type
 * \verbatim

 #include <fipa_acl/fipa_acl.hpp>
 using namespace fipa::acl;

 ACLMessage message;
 message.setPerformative(ACLMessage::REQUEST_WHEN);
 ...
 std::string encodedMsgData = MessageGenerator::create(message, fipa::acl::representation::STRING_REP);

 ...

 ACLMessage decodedMsg;
 MessageParser::parseData(encodedMsgData, decodedMsg, fipa::acl::representation::STRING_REP);
 \endverbatim
 *
 * \subsection A Generation and parsing of an envelope
 * \verbatim

 #include <fipa_acl/fipa_acl.hpp>

 using namespace fipa::acl;

 ACLMessage message;
 message.setPerformative(ACLMessage::REQUEST_WHEN);
 ...

 ACLEnvelope envelope(message, representation::BITEFFICIENT);

 std::string envelopeAndMsgData = EnvelopeGenerator::create(envelope, representation::BITEFFICIENT);

 ...
 ACLEnvelope decodedEnvelope;
 EnvelopeParser::parseData(envelopeAndMsgData, decodedEnvelope, representation::BITEFFICIENT);

 // Changes to an envelope are added as overlay while being transported across message
 // transport services which add further information or update the receiver fields.
 //
 // Getting the base envelope and initial value
 ACLBaseEnvelope outBaseEnvelope = decodedEnvelope.getBaseEnvelope();
 // Getting the very last values, i.e. when all overlays have been applied in the right
 // order
 ACLBaseEnvelope outFlattenedEnvelope = decodedEnvelope.flattened();
 \endverbatim
 *
 *
 * \section B Available bindings
 *
 * Ruby bindings exist for using fipa acl
 * \verbatim
 require 'fipa-message'
 include FIPA
 msg = ACLMessage.new
 msg.setPerformative :request_whenever
 msg.setProtocol "test"

 sender = AgentID.new 'sender'
 msg.setSender(sender)


 \endverbatim
 *
 */
#endif //FIPA_ACL_H
