#include "bitefficient_format.h"
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <base-logging/Logging.hpp>

namespace fipa {
namespace acl {

std::string BitefficientFormat::getString(const std::string& sword)
{
    // StringLiteral | ByteLengthEncodedString
    return getByteLengthEncodedString(sword);
}

std::string BitefficientFormat::getByteLengthEncodedString(const std::string& sword)
{
    char digitString[100];
    int digit = sword.size();	
    snprintf(digitString,100, "#%d%c",digit,'\"');
    
    return digitString + sword;
}

std::string BitefficientFormat::getNullTerminatedString(const std::string& string)
{
    return getByteLengthEncodedString(string) + char(0x00);
}

std::string BitefficientFormat::getBinDateTimeToken(const std::string& date1)
{
    return char(0x20) + getBinDate(date1);
}

std::string BitefficientFormat::getBinDateTimeToken(const base::Time& time)
{
    return char(0x20) + getBinDate(time);
}

std::string BitefficientFormat::getBinDate(const base::Time& baseTime)
{

    std::string time = baseTime.toString(base::Time::Milliseconds);

    // Input format should be "%Y%m%d-%H:%M:%S"
    // Strip ':' and '-' to allow from_iso_string to work
    boost::erase_all(time,":");
    boost::erase_all(time,"-");

    // extend millisecond field to 4 digits
    time.insert(14,sizeof(char),'0');

    return getBinDate(time);
}

std::string BitefficientFormat::getBinDate(const std::string& date1)
{
    std::string retstr;
    unsigned int i;
    for (i = 0; i < date1.length(); i = i + 2)
    {
        retstr = retstr + getCodedNaturalNumber(date1.substr(i,2));
    }
    
    return retstr;
}

std::string BitefficientFormat::getCodedNumber(const std::string& cn)
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

std::string BitefficientFormat::getCodedNaturalNumber(const std::string& cn)
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

std::string BitefficientFormat::getBinNumber(uint32_t number, bool asHex)
{
    std::string encoded;
    if(asHex)
    {
        encoded += char(0x13);
    } else {
        encoded += char(0x12);
    }

    std::stringstream ss;
    ss << number;

    encoded += getDigits(ss.str());
    return encoded;
}

std::string BitefficientFormat::getDigits(uint32_t number)
{
    std::stringstream ss;
    ss << number;

    return getDigits(ss.str());
}

std::string BitefficientFormat::getACLRepresentation(const representation::Type& type)
{
    std::string msg = "";
    switch(type)
    {
        case representation::BITEFFICIENT: 
            msg += char(0x10); break;
        case representation::STRING_REP:
            msg += char(0x11); break;
        case representation::XML:
            msg += char(0x12); break;
        default:
            throw std::runtime_error("BitefficientFormat: Cannot encode unknown acl representation");
    }

    return msg;
}

std::string BitefficientFormat::getReceivedObject(const ReceivedObject& receivedObject)
{
    // by
    std::string receivedObjectString = getNullTerminatedString(receivedObject.getBy());
    // date
    receivedObjectString += getBinDateTimeToken(receivedObject.getDate());
    // from
    std::string from = receivedObject.getFrom();
    if(!from.empty())
    {
        receivedObjectString += char(0x02) + getNullTerminatedString(from);
    }
    // id
    std::string id = receivedObject.getId();
    if(!id.empty())
    {
        receivedObjectString += char(0x03) + getNullTerminatedString(id);
    }
    // via
    std::string via = receivedObject.getVia();
    if(!via.empty())
    {
        receivedObjectString += char(0x04) + getNullTerminatedString(via);
    }
    
    const UserdefinedParameterList& list = receivedObject.getUserdefinedParameters();
    std::vector<UserdefParam>::const_iterator cit = list.begin();
    for(; cit != list.end(); ++cit)
    {
        receivedObjectString += char(0x00) + getNullTerminatedString(cit->getName()) + getNullTerminatedString(cit->getValue());
    }

    receivedObjectString += getEOFCollection();

    return receivedObjectString;
}

std::string BitefficientFormat::getAgentID(const AgentID& agentId)
{
    std::string agentIdString = char(0x02) + getNullTerminatedString(agentId.getName());
    // addresses (optional)
    {
        const Addresses& addresses = agentId.getAddresses();
        if(!addresses.empty())
        {
            Addresses::const_iterator cit = addresses.begin();
            agentIdString += char(0x02);
            for(; cit != addresses.end(); ++cit)
            {
                agentIdString +=  getNullTerminatedString(*cit);
            }
            agentIdString += getEOFCollection();
        }
    }
    // resolvers(optional)
    {
        const Resolvers& resolvers = agentId.getResolvers();
        if(!resolvers.empty())
        {
            Resolvers::const_iterator cit = resolvers.begin();
            agentIdString += char(0x03);
            for(; cit != resolvers.end(); ++cit)
            {
                agentIdString += getAgentID(*cit);
            }
            agentIdString += getEOFCollection();
        }
    }
    // userdefined parameters
    {
        const UserdefinedParameterList& userdefParams = agentId.getUserdefParams();
        if(!userdefParams.empty())
        {
            UserdefinedParameterList::const_iterator cit = userdefParams.begin();
            for(; cit != userdefParams.end(); ++cit)
            {
                agentIdString += char(0x05);
                agentIdString += getNullTerminatedString(cit->getName());
                agentIdString += getBinStringNoCodetable(cit->getValue());
            }
        }
    }
    agentIdString += getEOFCollection();
    return agentIdString;
}

std::string BitefficientFormat::getAgentIDSequence(const std::vector<AgentID>& agentIds)
{
    std::vector<AgentID>::const_iterator cit = agentIds.begin();
    std::string sequence;
    for(;cit != agentIds.end(); ++cit)
    {
        sequence += getAgentID(*cit);
    }
    sequence += getEOFCollection();
    return sequence;
}

} // end namespace acl
} // end namespace fipa
