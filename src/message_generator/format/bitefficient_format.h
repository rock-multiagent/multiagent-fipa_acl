#ifndef FIPA_ACL_BITEFFICIENT_FORMAT_H
#define FIPA_ACL_BITEFFICIENT_FORMAT_H

#include <string>
#include <base/Time.hpp>
#include <fipa_acl/message_generator/acl_envelope.h>
#include <fipa_acl/message_generator/acl_message.h>
#include <fipa_acl/message_generator/types.h>

namespace fipa {
namespace acl {

class BitefficientFormat
{
public:
    /**
    \brief returns the very used end-of-collection marker
    */
    static char getEOFCollection() { return 0x01; }

    static char getEOFEnvelope() { return getEOFCollection(); }

    static std::string getByteLengthEncodedString(const std::string& sword);

    static std::string getNullTerminatedString(const std::string& sword);

    /**
     * Return encoded string
     */
    static std::string getString(const std::string& sword);

    /**
    \brief implements a coded number passed as a string
    it goes through it digit by digit(char by char)
    */
    static std::string getCodedNumber(const std::string& cn);
    
    /**
    \brief Allow proper generation of natural numbers, will remove any leading zeros
    */
    static std::string getCodedNaturalNumber(const std::string& cn);

    /**
    \brief implements the date time token production of the grammar; not complete(w.r.t. the specification) in functionality 
    implementing the messages without the rest of the architecture makes it difficult to anticipate when and how some productions 
    may/will be used so only a few of the productions were implemented and the char parameter was added to choose between them, 
    as no other decission maker/constraint was identified
    */
    static std::string getBinDateTimeToken(const std::string& date1);
    
    /**
    \brief implements the date time token production of the grammar; not complete(w.r.t. the specification) in functionality 
    implementing the messages without the rest of the architecture makes it difficult to anticipate when and how some productions 
    may/will be used so only a few of the productions were implemented and the char parameter was added to choose between them, 
    as no other decission maker/constraint was identified
    */
    static std::string getBinDateTimeToken(const base::Time& time);
   
    /**
     * Retrieve bitefficient encoding for base time
     */
    static std::string getBinDate(const base::Time& time);

    /**
    \brief takes the string representing the date and passes it's digits 2 by 2(as length 2 sugstrings) to the byte encoding function
    it did not explicitly specify but it was induced from the way it was stated that the date is to be encoded as a coded number(comment 9 of the specification)
    */
    static std::string getBinDate(const std::string& date1);

    /**
     * Retrieve number string
     */
    static std::string getBinNumber(uint32_t number, bool asHex = false);

    /**
     * Encode representation
     */
    static std::string getACLRepresentation(const representation::Type& type);

    /**
     * Retrieve digits for a number string
     */
    static std::string getDigits(const std::string& numberString)  { return getCodedNumber(numberString); }

    /**
     * Get digits from unsigned integer
     */
    static std::string getDigits(uint32_t number);

    /**
     * Retrieve encoded received object
     */
    static std::string getReceivedObject(const ReceivedObject& object);

    /**
     * Retrieve encoded agent id for envelopes
     */
    static std::string getAgentID(const AgentID& agentId);

    /**
     * Get sequence of agents ids
     */
    static std::string getAgentIDSequence(const std::vector<AgentID>& agentIds);

    /**
     *
     */
    static std::string getBinStringNoCodetable(const std::string& sword) { return char(0x14) + getNullTerminatedString(sword); }

};

} // end namespace acl
} // end namespace fipa

#endif // FIPA_ACL_BITEFFICIENT_FORMAT_H
