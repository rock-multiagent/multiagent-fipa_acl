#include "bitefficient_format.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <iterator>
#include <stdexcept>
#include <arpa/inet.h>
#include <limits>

#include<boost/algorithm/string/erase.hpp>

#include <base/logging.h>

#include <fipa_acl/message_generator/acl_message.h>
#include <fipa_acl/message_generator/message_format.h>

namespace fipa {
namespace acl {

std::string BitefficientFormat::apply(const ACLMessage& msg) const
{

	std::string mes = getBitHeader() + getBitMessageType(msg) + getBitMessageParameters(msg);

        // Since content can be of arbitrary sicne add content here
        // to avoid copying around large content object
        const std::string* content = msg.getContentPtr();
        if (!content->empty())
        {
            uint32_t digit = content->size();	
            mes += 0x04;

            // Check if there is binary content to be set
            // add choose encoding based on the respective content size
            if(strlen(content->c_str()) != digit)
            {
                if(digit <= std::numeric_limits<uint8_t>::max())
                {
                    uint8_t dataSize = digit;
                    mes += 0x16;
                    mes.append(reinterpret_cast<const char*>(&dataSize), sizeof(uint8_t));
                    mes += *content;
                } else if (digit <= std::numeric_limits<uint16_t>::max())
                {
                    uint16_t dataSize = digit;
                    dataSize = htons(dataSize);
                    mes += 0x17;
                    mes.append(reinterpret_cast<const char*>(&dataSize),sizeof(uint16_t));
                    mes += *content;
                } else {
                    digit = htonl(digit);
                    mes += 0x19;
                    mes.append(reinterpret_cast<const char*>(&digit),sizeof(uint32_t));
                    mes += *content;
                }
            } else { // use string with null terminated (more efficient)
                char digitString[100];
                snprintf(digitString,100, "%c#%d%c",0x14,digit,'\"');
                mes += digitString;
                mes += *content;
                mes += char(0x00);
            }
        }

        mes += getBitEndOfColl();
	return mes;
}

char BitefficientFormat::getBitEndOfColl() const
{
	return char(0x01); 
}

std::string BitefficientFormat::getBitHeader() const
{	
	std::string retstr;
	
	char id = getBitMessageID();
	char version = getBitMessageVersion();
	
	return retstr.append(1,id).append(1,version);
}

char BitefficientFormat::getBitMessageID() const
{	
    if (!getUseCodeTables())
    {
        return 0xfa;
    }
    if (getUpdateCodeTables())
    {
        return 0xfb;
    }
    return 0xfc;
}

char BitefficientFormat::getBitMessageVersion() const
{
    std::string version = getVersion();
    return char(((int) version[0]-48)*16 + ((int)version[2]-48));
}

std::string BitefficientFormat::getBitMessageType(const ACLMessage& msg) const
{
    std::string performative = msg.getPerformative();
    // Check if we have one of the predefined performatives
    for (int i = (int) ACLMessage::ACCEPT_PROPOSAL; i < (int) ACLMessage::END_PERFORMATIVE; ++i)
    {
        if (PerformativeTxt[(ACLMessage::Performative) i] == performative)
        { 				
            char a[2]; 
            a[0] = char(i+1); 
            a[1] = '\0'; 
            return std::string(a);
        }
    }

    // actually we will have user defined performative here
    if(performative.empty())
        throw MessageGeneratorException("Performative cannot be empty");
	
    return (char(0x00) + getBitBinWord(performative));
} 

std::string BitefficientFormat::getBitBinWord(const std::string& sword) const
{
    if (!getUseCodeTables())
    {
        return (char(0x10) + sword + char(0x00));
    }

    throw std::runtime_error("BitefficientFormat does not support codetables");
}

std::string BitefficientFormat::getBitMessageParameters(const ACLMessage& msg) const
{
	return (getBitPredefMessageParams(msg) + getBitUserdefMessageParams(msg));
}



std::string BitefficientFormat::getBitPredefMessageParams(const ACLMessage& msg) const
{
    std::string retstr;

    // Check if entries exists for the predefined message parameters
    // sender is not always required, 
    AgentID sender = msg.getSender();
    if ( !sender.empty()) 
    {
        LOG_DEBUG("Writing sender: %s", getBitAID(sender, getResolverDepth()).c_str());
        retstr = retstr + char(0x02) + getBitAID(sender, getResolverDepth()); 
    }

    std::vector<AgentID> receivers = msg.getAllReceivers();
    if (!receivers.empty())
    {
        retstr = retstr + char(0x03) + getBitAIDColl(receivers,getResolverDepth()); 
    }

    ///** Content added at higher level to minimize copies of large content objects **/

    std::string replyWith = msg.getReplyWith();	
    if (!replyWith.empty())
        retstr = retstr + char(0x05) + getBitBinExpression(replyWith,'s'); 

    if (!msg.getReplyBy().isNull())
    {
        retstr = retstr + char(0x06) + getBitBinDateTimeToken(msg.getReplyBy()); 
    }
            
    std::string inReplyTo = msg.getInReplyTo();
    if (!inReplyTo.empty())
    {
        retstr = retstr + char(0x07) + getBitBinExpression(inReplyTo,'s'); 
    }

    std::vector<AgentID> replyTo = msg.getAllReplyTo();
    if (!replyTo.empty()) 
    {
        retstr = retstr + char(0x08) + getBitAIDColl(replyTo,getResolverDepth());
    }

    std::string language = msg.getLanguage();
    if (!language.empty())
    {
        retstr = retstr + char(0x09) + getBitBinExpression(language,'s'); 
    }

    std::string encoding = msg.getEncoding();
    if (!encoding.empty())
    {
        retstr = retstr + char(0x0a) + getBitBinExpression(encoding,'s');
    }

    std::string ontology = msg.getOntology();
    if (!ontology.empty())
        retstr = retstr + char(0x0b) + getBitBinExpression(ontology,'s'); 

    std::string protocol = msg.getProtocol();
    if (!protocol.empty())
    {
        retstr = retstr + char(0x0c) + getBitBinWord(protocol);
    }

    std::string conversationID = msg.getConversationID();
    if (!conversationID.empty())
        retstr = retstr + char(0x0d) + getBitBinExpression(conversationID,'s');
    
    return retstr;
}

std::string BitefficientFormat::getBitUserdefMessageParams(const ACLMessage& msg) const
{
    std::string retstr;

    std::vector<UserdefParam> s = msg.getUserdefParams();
    if( s.empty() )
    {
        return retstr;
    }

    LOG_WARN("Userdefined parameters: %d", (int) s.size());
  
    std::vector<UserdefParam>::iterator it; 
    it = s.begin();
    for (; it != s.end(); ++it)
    {
        retstr = retstr + char(0x00) + bitParseParam(*it);
    }

    return retstr;
}

std::string BitefficientFormat::getBitUserdefParams(const std::vector<UserdefParam>& params) const
{
    std::string retstr;
    std::vector<UserdefParam>::const_iterator it = params.begin();
    for (; it != params.end(); ++it)
        retstr = retstr + char(0x04) + bitParseParam(*it);

    return retstr;
}

std::string BitefficientFormat::bitParseParam(const UserdefParam& p) const
{
    return getBitBinWord(p.getName()) + getBitBinExpression(p.getValue(),'s');
}


std::string BitefficientFormat::getBitAID(const AgentID& aid, int depth) const
{
    if (depth > 0)
        return char(0x02) + getBitBinWord(aid.getName())+
            (( aid.getAddresses().empty() == true)? "" : (char(0x02) + getBinURLCol(aid.getAddresses()))) +
            (( aid.getResolvers().empty() == true)? "" : getBitResolvers(aid.getResolvers(),depth-1)) +
            (( aid.getUserdefParams().empty() == true)? "" : getBitUserdefParams(aid.getUserdefParams())) +
            getBitEndOfColl();
             
                         
    return char(0x02) + getBitBinWord(aid.getName())+
            (( aid.getAddresses().empty() == true)? "" : (char(0x02) + getBinURLCol(aid.getAddresses()))) + 
            (( aid.getUserdefParams().empty() == true)? "" : getBitUserdefParams(aid.getUserdefParams())) +
            getBitEndOfColl();
}


std::string BitefficientFormat::getBinURLCol(const std::vector<std::string>& adrr) const
{
    std::string retstr; 
    std::vector<std::string>::const_iterator it = adrr.begin();
    for (; it != adrr.end(); ++it)
        retstr = retstr + getBitBinWord(*it);

    retstr = retstr + getBitEndOfColl();
    return retstr;
}

std::string BitefficientFormat::getBitResolvers(const std::vector<AgentID>& aids,int depth) const
{
    return (char(0x03) + getBitAIDColl(aids,depth));
}

std::string BitefficientFormat::getBitAIDColl(const std::vector<AgentID>& aids, int depth) const
{
    std::string retstr;
    std::vector<AgentID>::const_iterator it = aids.begin();
    for (; it != aids.end(); ++it)
        retstr = retstr + getBitAID(*it, depth);
    return retstr + getBitEndOfColl();
}

std::string BitefficientFormat::getByteLengthEncodedString(const std::string& sword) const
{
    char digitString[100];
    int digit = sword.size();	
    snprintf(digitString,100, "%c#%d%c",0x14,digit,'\"');

    return digitString + sword + char(0x00);
}

std::string BitefficientFormat::getBitBinExpression(const std::string& sword,char c) const
{
    //if (!sword.compare(msg.getContent())) 
    //{
    //    return char(0xff) + getBitBinString(sword);
    //}

    if (c == 's') 
        return getBitBinString(sword);

    if (c == 'w') 
        return getBitBinWord(sword);

    throw std::runtime_error("BitefficientFormat: getBitBinExpression called with wrong base character");
}

std::string BitefficientFormat::getBitBinExpression(double n,char base) const
{
    return getBitBinNumber(n,base);
}

std::string BitefficientFormat::getBitBinNumber(double n,char base) const
{
    std::stringstream ss (std::stringstream::in | std::stringstream::out);
    ss << n;
    std::string test = ss.str();  

    if(base == 'h') 
        return char(0x13) + getBitDigits(test);
    if(base == 'o' || base == 'd') 
        return char(0x12) + getBitDigits(test);

    throw std::runtime_error("BitefficientFormat: getBitBinNumber called with wrong base character");
}

std::string BitefficientFormat::getBitBinString(const std::string& sword) const
{
    if (!getUseCodeTables())
    {
        return char(0x14) + ( '\"' + sword + '\"') + char(0x00);
    }
        // char(0x15) + return getCTIndex(sword);
    throw std::runtime_error("BitefficientFormat does not support codetables");

}

std::string BitefficientFormat::getBitDigits(const std::string& dig) const
{
    return getBitCodedNumber(dig);
}

std::string BitefficientFormat::getBitBinDateTimeToken(const std::string& date1) const
{
    return char(0x20) + getBitBinDate(date1);
}

std::string BitefficientFormat::getBitBinDateTimeToken(const base::Time& time) const
{
    return char(0x20) + getBitBinDate(time);
}

std::string BitefficientFormat::getBitBinDate(const base::Time& baseTime) const
{

    std::string time = baseTime.toString(base::Time::Milliseconds);

    // Input format should be "%Y%m%d-%H:%M:%S"
    // Strip ':' and '-' to allow from_iso_string to work
    boost::erase_all(time,":");
    boost::erase_all(time,"-");

    // extend millisecond field to 4 digits
    time.insert(14,sizeof(char),'0');

    return getBitBinDate(time);
}

std::string BitefficientFormat::getBitBinDate(const std::string& date1) const
{
    std::string retstr;
    unsigned int i;
    for (i = 0; i < date1.length(); i = i + 2)
    {
        retstr = retstr + getBitCodedNaturalNumber(date1.substr(i,2));
    }
    
    return retstr;

}

std::string BitefficientFormat::getBitCodedNumber(const std::string& cn) const
{
    std::string retstr;
    unsigned int i;
    char code(0x00);
    size_t decimal_size = cn.length();

    for(i = 0; i < decimal_size; i++)
    {
        if ((cn[i]>='0') && (cn[i]<='9')) code = char(code + 1 + int(cn[i]) - 48);
        if (cn[i] == '+') code = char(code + 12);
        if ((cn[i] == 'E') || (cn[i] == 'e')) code = char(code + 13);
        if (cn[i] == '-') code = char(code + 14);
        if (cn[i] == '.') code = char(code + 15);
        
        if (i%2 == 0)
        {
            code = code<<4;
        } else {
            retstr += code;
            code = char(0x00);
        }
    }

    if (decimal_size % 2 != 0)
    {
        retstr += code;
    } else {
        retstr += char(0x00);
    }
     
    return retstr;
}

std::string BitefficientFormat::getBitCodedNaturalNumber(const std::string& cn) const
{
    std::string retstr;

    unsigned int i;
    char code(0x00);
    size_t decimal_size = cn.length();

    for(i = 0; i < decimal_size; ++i)
    {
        switch(cn[i])
        {
            // Ignoring leading zeros here, 
            case '0': code += char(0x01); break;
            case '1': code += char(0x02); break;
            case '2': code += char(0x03); break;
            case '3': code += char(0x04); break;
            case '4': code += char(0x05); break;
            case '5': code += char(0x06); break;
            case '6': code += char(0x07); break;
            case '7': code += char(0x08); break;
            case '8': code += char(0x09); break;
            case '9': code += char(0x0a); break;
            //case '+': code += char(0x0c); break;
            //case 'e': 
            //case 'E': code += char(0x0d); break;
            //case '-': code += char(0x0e); break;
            //case '.': code += char(0x0f); break;
            default: 
                      assert(false);
        }

          
        if (i%2 == 0)
        { 
            code = code<<4;
        } else {
            retstr += code; 
            code = char(0x00); 
        }
    }

    if (i%2 != 0)
    {
        retstr += code;
    }
     
    return retstr;
}

} // end namespace acl
} // end namespace fipa
