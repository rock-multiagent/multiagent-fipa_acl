#ifndef FIPA_ACL_BITEFFICIENT_FORMATTER_H
#define FIPA_ACL_BITEFFICIENT_FORMATTER_H

#include <fipa_acl/message_generator/message_format.h>

namespace fipa {
namespace acl {

class BitefficientMessageFormat : public MessageFormat
{
public:
    /**
     * Applies the format to the message
     * \return the formatted message object
     */
    std::string apply(const ACLMessage& msg) const;

    /**
        \brief encodes an AgentID instance
        
        as some fields are optional the method first checks for their existance
        it also keeps record of the res_depth
        
        \param aid: AgentID object pointer to be encoded
        \param depth: depth of the resolver tree to be encoded
        \return the encoded agentAID as string
    */
    std::string getBitAID(const AgentID& aid, int depth) const;
    
    /**
    \brief returns the very used end-of-collection marker
    */
    char getBitEndOfColl() const;

    /**
    \brief puts the header toghether
    */
    std::string  getBitHeader() const;

    /**
    \brief depending on the flags it sets the ID
    */
    char  getBitMessageID() const;

    /**
    \brief gets the message version in one byte
    the message version has a very specific format: digit+dot+digit
    */
    char getBitMessageVersion() const;

    /**
    \brief encodes the message performative;
    it compares the string of the message performative to all the predefined ones; if it is one of them it returns accordingly if not it returns the 
    custom performative
    */
    std::string getBitMessageType(const ACLMessage& msg) const;
    
    /**
    \brief quite frequently used production;
    * currently it does not have complete functionality: the code tables need to be implemented in order to use them
    */
    std::string getBitBinWord(const std::string& sword) const;
    
    /** 
    \brief puts together all the message parameters 
    */
    std::string getBitMessageParameters(const ACLMessage& msg) const;
    
    /**	
    \brief checks all predefined message parameters whether they are set or not and it encodes them accordingly
    */
    std::string getBitPredefMessageParams(const ACLMessage& msg) const;
    
    /**
    parses all the user defined parameters of the message if any
    */
    std::string getBitUserdefMessageParams(const ACLMessage& msg) const;
    
    /**
    \brief parses a set of user defined parameters;
    different from the getBitUserdefMessageParams() method in that there is more general; the former is to be called only for message parameters;
    difference was imposed by the specification	
    */
    std::string getBitUserdefParams(const std::vector<UserdefParam>& params) const;
    
    /**
    \brief parses one user defined parameter
    the specification does not differentiate at this level between message parameters and other kinds so this method is called by both
    */
    std::string bitParseParam(const UserdefParam&) const;
    
    /**
    \brief currently used to encode the addresses of the AgentID instances(strings)
    */
    std::string getBinURLCol(const std::vector<std::string>&) const;
    
    /**
    \brief basically a wrapper function of the getBitAIDColl(), to add the required specific flag
    */
    std::string getBitResolvers(const std::vector<AgentID>& aids,int depth) const;
    
    /**
    \brief parses a set of AgentID instances
    the resolvers depth variable that is being passed around is not modified in this function
    */
    std::string getBitAIDColl(const std::vector<AgentID>& aids, int depth) const;
    
    /**
    \brief implements the binary expression production of the grammar; not complete(w.r.t. the specification) in functionality
    implementing the messages without the rest of the architecture makes it difficult to anticipate when and how some productions 
    may/will be used so only a few of the productions were implemented(for the binary expression) and the char parameter was added 
    to choose between them, as no other decission maker/constraint was identified
    */
    std::string getBitBinExpression(const std::string& sword,char c) const;
    
    /**
    \brief overloaded version of the above function
    */
    std::string getBitBinExpression(double n,char base) const;
    
    /**
    \brief parses a number according to the specification(see comment 9 of the specification)
    for ease it is turned into a string and passed on
    */
    std::string getBitBinNumber(double n,char base) const;
    
    /**
    * Retrieve the byte length encoded string for the given word 
    * \param sword String containing the data to convert
    */
    std::string getByteLengthEncodedString(const std::string& sword) const;
    
    /**
    \brief implements the binary string production; functionality not complete as code tables are not yet implemented
    the second argument is an explicit option for codeTables(needed for the getBitBinExpression() )
    */
    std::string getBitBinString(const std::string& sword) const;
   
    /**
     * Get bitefficient representation of digits
     */
    std::string getBitDigits(const std::string& dig) const;
    
    /**
    \brief implements the date time token production of the grammar; not complete(w.r.t. the specification) in functionality 
    implementing the messages without the rest of the architecture makes it difficult to anticipate when and how some productions 
    may/will be used so only a few of the productions were implemented and the char parameter was added to choose between them, 
    as no other decission maker/constraint was identified
    */
    std::string getBitBinDateTimeToken(const std::string& date1) const;
    
    /**
    \brief implements the date time token production of the grammar; not complete(w.r.t. the specification) in functionality 
    implementing the messages without the rest of the architecture makes it difficult to anticipate when and how some productions 
    may/will be used so only a few of the productions were implemented and the char parameter was added to choose between them, 
    as no other decission maker/constraint was identified
    */
    std::string getBitBinDateTimeToken(const base::Time& time) const;
   
    /**
     * Retrieve bitefficient encoding for base time
     */
    std::string getBitBinDate(const base::Time& time) const;

    /**
    \brief takes the string representing the date and passes it's digits 2 by 2(as length 2 sugstrings) to the byte encoding function
    it did not explicitly specify but it was induced from the way it was stated that the date is to be encoded as a coded number(comment 9 of the specification)
    */
    std::string getBitBinDate(const std::string& date1) const;
    
    /**
    \brief implements a coded number passed as a string
    it goes through it digit by digit(char by char)
    */
    std::string getBitCodedNumber(const std::string& cn) const;
    
    /**
    \brief Allow proper generation of natural numbers, will remove any leading zeros
    */
    std::string getBitCodedNaturalNumber(const std::string& cn) const;
};

} // end namespace acl
} // end namespace fipa

#endif // FIPA_ACL_BITEFFICIENT_FORMATTER_H
