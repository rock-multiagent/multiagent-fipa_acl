/**
*
* \file format.h
* \author Mircea Cretu Stancu
* \brief Encodes a given ACLMessage according to the fipa Bit-Efficent encoding speciffication(FIPA at http://www.fipa.org).
*/

#ifndef FIPA_ACL_ACLMESSAGE_MESSAGE_FORMAT_H
#define FIPA_ACL_ACLMESSAGE_MESSAGE_FORMAT_H
#include <fipa_acl/message_generator/acl_message.h>
#include <fipa_acl/message_generator/exception.h>

namespace fipa {
namespace acl {
    
/**
* \class MessageFormat
* \brief encodes and ACLMessage class object according to the fipa spec(SC00069) all pointer extractin/passing methods are 
* shallow(only the references passed); this is ok as they are not modiffied in the process of encoding; if this changes as 
* utility increases(objects will be modified) then change to deep-copy retreival should be trivial, with the overloaded operators
*/
class MessageFormat 
{

private:
     /**
     @msg message to be parsed

     @useCodeTables flag to determine whether we use code tables or not
     * sice the code tables are not implemented yet, it's value is initialized with 0 and is not to be changed

     @updateCodeTables flag to determine whether we update the code tables
     * it does not have any practical relevance unless useCodeTables = 1

     @version string that keeps the version of the encoder
     * needed by the speciffication; initialized to "1.0"
     * important restraint: as the speciffication states the version must be of strictly 2 digits;
     
     @res_depth a control variable that speciffies the depth of speciffing resolvers when encoding AgentID's
     * see speciffication for details 
     */
    ACLMessage mMessage;
    bool mUseCodeTables;
    bool mUpdateCodeTables;
    std::string mVersion;
    int mResolverDepth;
    
public:
    /**
     * Default construction
     */
    MessageFormat();

    /**
     * Set if use code table should be used
     */
    void setUseCodeTables(bool x) { mUseCodeTables = x; }

    /**
     * Check if code tables shouldbe used
     * \return True, if code tables should be used, false otherwise
     */
    bool getUseCodeTables() { return mUseCodeTables; }

    /**
     * Set whether code tables should be updated
     */
    void setUpdateCodeTables(bool x) { mUpdateCodeTables = x; }

    /**
     * Get update code tables
     * \return True if codetable should be update, false otherwise
     */
    bool getUpdateCodeTables() { return mUpdateCodeTables; }

    /**
     * Set resolver depth which should be considered for comparison
     */
    void setResolverDepth(int depth) { mResolverDepth = depth; }

    /**
     * Get resolver depth which should be considered for comparison
     * \return resolver depth
     */
    int getResolverDepth() { return mResolverDepth; }

    /**
     * Set version format
     */
    void setVersion(const std::string& version) { mVersion = version; }

    /**
     * Get version of the format
     */
    std::string getVersion() { return mVersion; }

    /**
     * Format the message 
     * \return the newly formatted message object
     */
    virtual std::string apply(const ACLMessage& msg) const = 0;
};


}//end of acl namespace
}// end of fipa namespace
#endif
