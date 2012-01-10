/**
 *
 * \file acl_message_output_parser.cpp
 * \author Mircea Cretu Stancu
 * \brief Encodes a given ACLMessage according to the fipa Bit-Efficent encoding speciffication(FIPA at http://www.fipa.org).
 * 
 * \version 1.0
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <iterator>
#include <stdexcept>
#include "acl_message_output_parser.h"
#include "acl_message.h"
#include "userdef_param.h"
#include "agent_id.h"
#include <base/logging.h>

namespace fipa {

namespace acl {
    

void ACLMessageOutputParser::setMessage(const ACLMessage &a)
{
	msg = a;
}


ACLMessageOutputParser::ACLMessageOutputParser()
{
	/// here we set the default values
       useCodeTables = 0;
       updateCodeTables = 1;
       version = "1.0";
       res_depth = 1; 
}


int ACLMessageOutputParser::printParsedMessage(const std::string& stream)
{
     std::ofstream out(stream.c_str(), std::ios_base::binary);
	if (!out)
		return 0;
	
	std::string output = getBitMessage();

	for (unsigned int i = 0; i < output.length(); i++)
		out.put(output[i]);

	out.close();
	return 1;
     
     
}


std::string ACLMessageOutputParser::getBitMessage()
{
	std::string mes = getBitHeader() + getBitMessageType() + getBitMessageParameters();

        // Since content can be of arbitrary sicne add content here
        // to avoid copying around large content object
        std::string* content = msg.getContentPtr();
        if (!content->empty())
        {
            char digitString[100];
            int digit = content->size();	
            snprintf(digitString,100, "%c%c#%d%c", 0x04,0x14,digit,'\"');

            mes += digitString;
            mes += *content;
            mes += char(0x00);
        }

        mes += getBitEndOfColl();
	return mes;
}

char ACLMessageOutputParser::getBitEndOfColl()
{
	return char(0x01); 
}

std::string ACLMessageOutputParser::getBitHeader()
{	
	std::string retstr;
	
	char id = getBitMessageID();
	char version = getBitMessageVersion();
	
	return retstr.append(1,id).append(1,version);
}

char ACLMessageOutputParser::getBitMessageID()
{	
	
    if (useCodeTables == 0) return 0xfa;
    if (updateCodeTables == 1) return 0xfb;
    return 0xfc;
}

char ACLMessageOutputParser::getBitMessageVersion()
{
	return char(((int)version[0]-48)*16 + ((int)version[2]-48));
}

std::string ACLMessageOutputParser::getBitMessageType()
{
    // Check if we have one of the predefined performatives
    for (int i = (int) ACLMessage::ACCEPT_PROPOSAL; i < (int) ACLMessage::END_PERFORMATIVE; i++)
    {
        if (PerformativeTxt[(ACLMessage::Performative) i] == msg.getPerformative())
        { 				
            char a[2]; 
            a[0] = char(i+1); 
            a[1] = '\0'; 
            return std::string(a);
        }
    }

    // actually we will have user defined performative here
    if(msg.getPerformative().empty())
        throw MessageGeneratorException("Performative cannot be empty");
	
    return (char(0x00) + getBitBinWord(msg.getPerformative()));
} 

std::string ACLMessageOutputParser::getBitBinWord(const std::string& sword)
{
    if (useCodeTables == 0) return (char(0x10) + sword + char(0x00));
       throw std::runtime_error("ACLMessageOutputParser does not support codetables");
       // return char(0x11) + getCTIndex(sword);
}

std::string ACLMessageOutputParser::getBitMessageParameters()
{
	         
	return (getBitPredefMessageParams() + getBitUserdefMessageParams());
}

std::string ACLMessageOutputParser::getBitPredefMessageParams()
{
    std::string retstr;

    // Check if entries exists for the predefined message parameters
    // sender is not always required, 
    AgentID sender = msg.getSender();
    if ( !sender.empty()) 
        retstr = retstr + char(0x02) + getBitAID(sender, res_depth); 

    std::vector<AgentID> receivers = msg.getAllReceivers();
    if (!receivers.empty())
        retstr = retstr + char(0x03) + getBitAIDColl(receivers,res_depth); 

    /** Content added at higher level to minimize copies of large content objects **/

    std::string replyWith = msg.getReplyWith();	
    if (!replyWith.empty())
        retstr = retstr + char(0x05) + getBitBinExpression(replyWith,'s'); 

    std::string replyBy = msg.getReplyBy1(0);
    if (!replyBy.empty())
        retstr = retstr + char(0x06) + getBitBinDateTimeToken(replyBy); 
            
    std::string inReplyTo = msg.getInReplyTo();
    if (!inReplyTo.empty())
        retstr = retstr + char(0x07) + getBitBinExpression(inReplyTo,'s'); 

    std::vector<AgentID> replyTo = msg.getAllReplyTo();
    if (!replyTo.empty()) 
        retstr = retstr + char(0x08) + getBitAIDColl(replyTo,res_depth);

    std::string language = msg.getLanguage();
    if (!language.empty())
        retstr = retstr + char(0x09) + getBitBinExpression(language,'s'); 

    std::string encoding = msg.getEncoding();
    if (!encoding.empty())
        retstr = retstr + char(0x0a) + getBitBinExpression(encoding,'s');

    std::string ontology = msg.getOntology();
    if (!ontology.empty())
        retstr = retstr + char(0x0b) + getBitBinExpression(ontology,'s'); 

    std::string protocol = msg.getProtocol();
    if (!protocol.empty())
        retstr = retstr + char(0x0c) + getBitBinWord(protocol);

    std::string conversationID = msg.getConversationID();
    if (!conversationID.empty())
        retstr = retstr + char(0x0d) + getBitBinExpression(conversationID,'s');
            
    return retstr;
}

std::string ACLMessageOutputParser::getBitUserdefMessageParams()
{
    std::string retstr;

    std::vector<UserdefParam> s = msg.getUserdefParams();
    if( s.empty() )
        return retstr;

    LOG_WARN("Userdefined parameters: %d", (int) s.size());
  
    std::vector<UserdefParam>::iterator it; 
    it = s.begin();
    for (; it != s.end(); ++it)
        retstr = retstr + char(0x00) + bitParseParam(*it);

    return retstr;
}

std::string ACLMessageOutputParser::getBitUserdefParams(const std::vector<UserdefParam>& params)
{
    std::string retstr;
    std::vector<UserdefParam>::const_iterator it = params.begin();
    for (; it != params.end(); ++it)
        retstr = retstr + char(0x04) + bitParseParam(*it);

    return retstr;
}

std::string ACLMessageOutputParser::bitParseParam(const UserdefParam& p)
{
    return getBitBinWord(p.getName()) + getBitBinExpression(p.getValue(),'s');
}


std::string ACLMessageOutputParser::getBitAID(const AgentID& aid, int depth)
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


std::string ACLMessageOutputParser::getBinURLCol(const std::vector<std::string>& adrr)
{
    std::string retstr = std::string();
    std::vector<std::string>::const_iterator it = adrr.begin();
    for (; it != adrr.end(); ++it)
        retstr = retstr + getBitBinWord(*it);

    retstr = retstr + getBitEndOfColl();
    return retstr;
}

std::string ACLMessageOutputParser::getBitResolvers(const std::vector<AgentID>& aids,int depth)
{
    return (char(0x03) + getBitAIDColl(aids,depth));
}

std::string ACLMessageOutputParser::getBitAIDColl(const std::vector<AgentID>& aids, int depth)
{
    std::string retstr;
    std::vector<AgentID>::const_iterator it = aids.begin();
    for (; it != aids.end(); ++it)
        retstr = retstr + getBitAID(*it, depth);
    return retstr + getBitEndOfColl();
}

std::string ACLMessageOutputParser::getByteLengthEncodedString(const std::string& sword)
{
    char digitString[100];
    int digit = sword.size();	
    snprintf(digitString,100, "%c#%d%c",0x14,digit,'\"');

    return digitString + sword + char(0x00);
}

std::string ACLMessageOutputParser::getBitBinExpression(const std::string& sword,char c)
{
    if (!sword.compare(msg.getContent())) 
        return char(0xff) + getBitBinString(sword,0);

    if (c == 's') 
        return getBitBinString(sword);

    if (c == 'w') 
        return getBitBinWord(sword);

    throw std::runtime_error("ACLMessageOutputParser: getBitBinExpression called with wrong base character");
}

std::string ACLMessageOutputParser::getBitBinExpression(double n,char base)
{
    return getBitBinNumber(n,base);
}

std::string ACLMessageOutputParser::getBitBinNumber(double n,char base)
{
    std::stringstream ss (std::stringstream::in | std::stringstream::out);
    ss << n;
    std::string test = ss.str();  

    if(base == 'h') 
        return char(0x13) + getBitDigits(test);
    if(base == 'o' || base == 'd') 
        return char(0x12) + getBitDigits(test);

    throw std::runtime_error("ACLMessageOutputParser: getBitBinNumber called with wrong base character");
}

std::string ACLMessageOutputParser::getBitBinString(const std::string& sword)
{
    //if (!useCodeTables) 
    return char(0x14) + ('\"' + sword + '\"') + char(0x00);
    // char(0x15) + return getCTIndex(sword);
}

std::string ACLMessageOutputParser::getBitBinString(const std::string& sword,int codeTables)
{
    if (!codeTables)
        return char(0x14) + ( '\"' + sword + '\"') + char(0x00);
        // char(0x15) + return getCTIndex(sword);
    throw std::runtime_error("ACLMessageOutputParser does not support codetables");

}

std::string ACLMessageOutputParser::getBitDigits(const std::string& dig)
{
    return getBitCodedNumber(dig);
}

std::string ACLMessageOutputParser::getBitBinDateTimeToken(const std::string& date1)
{
    return char(0x20) + getBitBinDate(date1);
}

std::string ACLMessageOutputParser::getBitBinDate(const std::string& date1)
{
    std::string retstr;
    unsigned int i;
    for (i = 0; i < date1.length(); i = i + 2)
    {
        retstr = retstr + getBitCodedNumberByte(date1.substr(i,2));
    }
    
    return retstr;

}

std::string ACLMessageOutputParser::getBitCodedNumber(const std::string& cn)
{
    std::string retstr;
    unsigned int i;
    char code = char(0x00);
    for(i = 0; i < cn.length(); i++)
    {
        if ((cn[i]>='0') && (cn[i]<='9')) code = char(code + 1 + int(cn[i]) - 48);
        if (cn[i] == '+') code = char(code + 12);
        if ((cn[i] == 'E') || (cn[i] == 'e')) code = char(code + 13);
        if (cn[i] == '-') code = char(code + 14);
        if (cn[i] == '.') code = char(code + 15);
        
        if (i%2 == 0) code = code<<4;
        if (i%2 == 1) {retstr = retstr + code; code = char(0x00); }
    }
    if (code!=char(0x00)) 
    {
        code = char(code + 0);
        retstr = retstr + code;
    } else {
        retstr = retstr + char(0x00);
    }
     
     return retstr;
}

std::string ACLMessageOutputParser::getBitCodedNumberByte(const std::string& cn)
{
    std::string retstr;

    unsigned int i;
    char code = char(0x00);
    for(i = 0; i < cn.length(); i++)
    {
        if ((cn[i]>='0') && (cn[i]<='9')) code = char(code + 1 + int(cn[i]) - 48);
        if (cn[i] == '+') code = char(code + 12);
        if ((cn[i] == 'E') || (cn[i] == 'e')) code = char(code + 13);
        if (cn[i] == '-') code = char(code + 14);
        if (cn[i] == '.') code = char(code + 15);
          
        if (i%2 == 0) code = code<<4;
        if (i%2 == 1) {retstr = retstr + code; code = char(0x00); }
    }
    if (code!=char(0x00)) 
    {
        code = char(code + 0);
        retstr = retstr + code;
    }
   // else retstr = retstr + char(0x00);
    return retstr;
}

void ACLMessageOutputParser::setUseCodeTables(int x){useCodeTables = x; }

int ACLMessageOutputParser::getUseCodeTables() {return useCodeTables; }

void ACLMessageOutputParser::setUpdateCodeTables(int x) {updateCodeTables = x;}

int ACLMessageOutputParser::getUpdateCodeTables() {return updateCodeTables;}

void ACLMessageOutputParser::setResolverDepth(int res){res_depth = res;}

int ACLMessageOutputParser::getResolverDepth() {return res_depth;}

void ACLMessageOutputParser::setVersion(const std::string& v){version = v; }

std::string ACLMessageOutputParser::getVersion() {return version; }

}//end of acl namespace

}// end of fipa namespace



