/**
 *
 * \file ACLMessageOutputParser.cpp
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
#include "ACLMessageOutputParser.h"
#include "ACLMessage.h"
#include "UserdefParam.h"
#include "AgentID.h"

namespace fipa {

namespace acl {
    

void ACLMessageOutputParser::setMessage(ACLMessage &a)
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


int ACLMessageOutputParser::printParsedMessage(std::string stream)
{
     std::ofstream out(stream.c_str(), std::ios_base::binary);
	if (!out)
		return 0;
	
	std::string output = getBitMessage();
	

	for (int i = 0; i < output.length(); i++){
		out.put(output[i]);
	//std::cout<<int(output[i])<<"  ";	
	}
	//std::cout<<std::endl;

	out.close();
	return 1;
     
     
}


std::string ACLMessageOutputParser::getBitMessage()
{
	
      std::string mes = std::string();
	mes.clear();

      mes = getBitHeader() + getBitMessageType() + getBitMessageParameters() + getBitEndOfColl();
	
	return mes;
}

char ACLMessageOutputParser::getBitEndOfColl()
{
	
	return char(0x01); 
}

std::string ACLMessageOutputParser::getBitHeader()
{	
	std::string retstr = std::string();
	retstr.clear();
	
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
            for (int i = 0; i < 22; i++)
                if (!ACLMessage::perfs[i].compare(msg.getPerformative()))
                              { 				
                                char a[2]; a[0] = char(i+1); a[1] = '\0'; 
                                return std::string(a);
                              }
		
            return (char(0x00) + getBitBinWord(msg.getPerformative()));
} 

std::string ACLMessageOutputParser::getBitBinWord(std::string sword)
{
		
	      if (useCodeTables == 0) return (char(0x10) + sword + char(0x00));
           // return char(0x11) + getCTIndex(sword);
}

std::string ACLMessageOutputParser::getBitMessageParameters()
{
	         
	return (getBitPredefMessageParams() + getBitUserdefMessageParams());
}

std::string ACLMessageOutputParser::getBitPredefMessageParams()
{
            std::string retstr = std::string();
	    retstr.clear();

	    // Check if entries exists for the predefined message parameters
	    // sender is not always required, 
            if ( !(msg.getSender().empty()) )
		retstr = retstr + char(0x02) + getBitAID(msg.getSender(), res_depth); 
            if (!(msg.getAllReceivers().empty()))
		 retstr =retstr + char(0x03) + getBitAIDColl(msg.getAllReceivers(),res_depth); 
            if (!msg.getContent().empty())
		retstr = retstr + char(0x04) + getBitBinString(msg.getContent(),0); 
            if (!msg.getReplyWith().empty())
		 retstr = retstr + char(0x05) + getBitBinExpression(msg.getReplyWith(),'s'); 
            if (!msg.getReplyBy1(0).empty())
		 retstr = retstr + char(0x06) + getBitBinDateTimeToken(msg.getReplyBy1(0)); 
            if (!msg.getInReplyTo().empty())
		 retstr = retstr + char(0x07) + getBitBinExpression(msg.getInReplyTo(),'s'); 
            if (!msg.getAllReplyTo().empty()) 
		 retstr = retstr + char(0x08) + getBitAIDColl(msg.getAllReplyTo(),res_depth);
            if (!msg.getLanguage().empty())
		 retstr = retstr + char(0x09) + getBitBinExpression(msg.getLanguage(),'s'); 
            if (!msg.getEncoding().empty())
		 retstr = retstr + char(0x0a) + getBitBinExpression(msg.getEncoding(),'s');
            if (!msg.getOntology().empty())
		 retstr = retstr + char(0x0b) + getBitBinExpression(msg.getOntology(),'s'); 
            if (!msg.getProtocol().empty())
		 retstr = retstr + char(0x0c) + getBitBinWord(msg.getProtocol());
            if (!msg.getConversationID().empty())
		 retstr = retstr + char(0x0d) + getBitBinExpression(msg.getConversationID(),'s');
            
            return retstr;
}

std::string ACLMessageOutputParser::getBitUserdefMessageParams()
{
            std::string retstr = std::string();
	    retstr.clear();

            std::vector<UserdefParam> s = msg.getUserdefParams();
	    if(s.empty()) return retstr;
	
            std::vector<UserdefParam>::iterator it; 
	    it = s.begin();
            for (it; it != s.end(); it++)
		retstr = retstr + char(0x00) + bitParseParam(*it);

            return retstr;
}

std::string ACLMessageOutputParser::getBitUserdefParams(std::vector<UserdefParam> params)
{
            std::string retstr = std::string();
            std::vector<UserdefParam>::iterator it = params.begin();
            for (it; it != params.end(); it++)
                retstr = retstr + char(0x04) + bitParseParam(*it);

            return retstr;
}

std::string ACLMessageOutputParser::bitParseParam(UserdefParam p)
{
		return getBitBinWord(p.getName()) + getBitBinExpression(p.getValue(),'s');
}


std::string ACLMessageOutputParser::getBitAID(AgentID aid, int depth)
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


std::string ACLMessageOutputParser::getBinURLCol(std::vector<std::string> adrr)
{
            std::string retstr = std::string();
            std::vector<std::string>::iterator it = adrr.begin();
            for (it; it != adrr.end(); it++)
                retstr = retstr + getBitBinWord(*it);
            retstr = retstr + getBitEndOfColl();
            
            return retstr;
}

std::string ACLMessageOutputParser::getBitResolvers(std::vector<AgentID> aids,int depth)
{
            return (char(0x03) + getBitAIDColl(aids,depth));
}

std::string ACLMessageOutputParser::getBitAIDColl(std::vector<AgentID> aids, int depth)
{
            std::string retstr = std::string();
            std::vector<AgentID>::iterator it = aids.begin();
            for (it; it != aids.end(); it++)
                retstr = retstr + getBitAID(*it, depth);
            return retstr + getBitEndOfColl();
}

std::string ACLMessageOutputParser::getBitBinExpression(std::string sword,char c)
{
            if (!sword.compare(msg.getContent())) return char(0xff) + getBitBinString(sword,0);
            if (c == 's') return getBitBinString(sword);
            if (c == 'w') return getBitBinWord(sword);
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
            if(base == 'h') return char(0x13) + getBitDigits(test);
            if(base == 'o' || base == 'd') return char(0x12) + getBitDigits(test);
}

std::string ACLMessageOutputParser::getBitBinString(std::string sword)
{
            if (!useCodeTables) return char(0x14) + ('\"' + sword + '\"') + char(0x00);
            // char(0x15) + return getCTIndex(sword);
}

std::string ACLMessageOutputParser::getBitBinString(std::string sword,int codeTables)
{
            if (!codeTables) return char(0x14) + ( '\"' + sword + '\"') + char(0x00);
            // char(0x15) + return getCTIndex(sword);
}

std::string ACLMessageOutputParser::getBitDigits(std::string dig)
{
           return getBitCodedNumber(dig);
}

std::string ACLMessageOutputParser::getBitBinDateTimeToken(std::string date1)
{
            return char(0x20) + getBitBinDate(date1);
}

std::string ACLMessageOutputParser::getBitBinDate(std::string date1)
{
            std::string retstr = std::string();
            int i;
            for (i = 0; i < date1.length(); i = i + 2)
            {
                
                retstr = retstr + getBitCodedNumberByte(date1.substr(i,2));
            }
            
            return retstr;

}

std::string ACLMessageOutputParser::getBitCodedNumber(std::string cn)
{
            std::string retstr = std::string();
            int i;
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
            if (code!=char(0x00)) {
                            code = char(code + 0);
                            retstr = retstr + code;
                           }
            else retstr = retstr + char(0x00);
            
            return retstr;
            
}

std::string ACLMessageOutputParser::getBitCodedNumberByte(std::string cn)
{
            std::string retstr = std::string();
            int i;
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
            if (code!=char(0x00)) {
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
void ACLMessageOutputParser::setVersion(std::string v){version = v; }
std::string ACLMessageOutputParser::getVersion() {return version; }

/*

bool operator== ( ACLMessage &a,  ACLMessage &b)
{
    
    //ACLMessage a = ACLMessage(&c); 
    //ACLMessage b = ACLMessage(&d);
        
    if (a.getPerformative().compare(b.getPerformative()))
        return false;
    if (a.getContent().compare(b.getContent()))
        return false;
    if (a.getLanguage().compare(b.getLanguage()))
        return false;
    if (a.getEncoding().compare(b.getEncoding()))
        return false;
    if (a.getOntology().compare(b.getOntology()))
        return false;
    if (a.getProtocol().compare(b.getProtocol()))
        return false;
    if (a.getConversationID().compare(b.getConversationID()))
        return false;
    if (a.getReplyWith().compare(b.getReplyWith()))
        return false;
    if (a.getInReplyTo().compare(b.getInReplyTo()))
        return false;
    if (a.getReplyBy1().compare(b.getReplyBy1()))
        return false;
    
    if (a.getSender() == b.getSender());
    else return false;
    
    // checking if receivers sets of the message are the same
    std::vector<AgentID> agentsA = a.getAllReceivers();
    std::vector<AgentID> agentsB = b.getAllReceivers();
    std::vector<AgentID>::iterator ait = agentsA.begin();
    std::vector<AgentID>::iterator bit = agentsB.begin();
    int found_one = 0; // flag variable to control flow through inner loops
    while (ait != agentsA.end())
    {
        found_one = 0;
        bit = agentsB.begin();
        while (bit != agentsB.end())
        {
	  if ( (*ait) == (*bit)) 
	  {
	      agentsA.erase(ait);
	      ait = agentsA.begin();
	      agentsB.erase(bit);
	      bit = agentsB.end();
	      found_one = 1;
	      
	  } else bit++;
	  
        }
        if (!found_one) ait++;
	  
    }
    if (!agentsA.empty())
        return false;
    if (!agentsB.empty())
        return false;
    
    //checking if reply_to sets of the message are  the same
    agentsA = a.getAllReplyTo();
    agentsB = b.getAllReplyTo();
    ait = agentsA.begin();
    bit = agentsB.begin();
    while (ait != agentsA.end())
    {
        found_one = 0;
        bit = agentsB.begin();
        while (bit != agentsB.end())
        {
	  if ( (*ait) == (*bit)) 
	  {
	      agentsA.erase(ait);
	      ait = agentsA.begin();
	      agentsB.erase(bit);
	      bit = agentsB.end();
	      found_one = 1;
	      
	  } else bit++;
	  
        }
        if (!found_one) ait++;
	  
    }
    if (!agentsA.empty())
        return false;
    if (!agentsB.empty())
        return false;
    
    
    std::vector<UserdefParam> paramsA = a.getUserdefParams();
    std::vector<UserdefParam> paramsB = b.getUserdefParams();
    std::vector<UserdefParam>::iterator pita = paramsA.begin();
    std::vector<UserdefParam>::iterator pitb = paramsB.begin();
     
    while (pita != paramsA.end())
    {
        found_one = 0;
        pitb = paramsB.begin();
        while (pitb!=paramsB.end())
        {
	  if ( (*pita) == (*pitb)) 
	  {
	      
	      paramsA.erase(pita);
	      pita = paramsA.begin();
	      paramsB.erase(pitb);
	      pitb = paramsB.end();
	      found_one = 1;
	  }
	  else pitb++;
        }
	  if (!found_one) pita++;
	 
	  
    }
    if (!paramsA.empty())
        return false;
    if (!paramsB.empty())
        return false;
   
    return true;
}


bool operator== ( AgentID &a,  AgentID &b)
{
    ///saving the resCompDepth variable for later restoration
    int depthRestore = AgentID::getResCompDepth();
    
    //AgentID a = AgentID(&c);
    //AgentID b = AgentID(&d);
    if (a.getName().compare(b.getName()))
        return false;
    
    // comparing addresses
    std::vector<std::string> addrA = a.getAddresses();
    std::vector<std::string> addrB = b.getAddresses();
    std::vector<std::string>::iterator sita = addrA.begin();
    std::vector<std::string>::iterator sitb = addrB.begin();
    int found_one = 0; // flag variable to control flow through inner loops
    while (sita != addrA.end())
    {
        found_one = 0;
        sitb = addrB.begin();
        while (sitb != addrB.end())
        {
	  if (!(*sita).compare(*sitb)) 
	  {
	      addrA.erase(sita);
	      sita = addrA.begin();
	      addrB.erase(sitb);
	      sitb = addrB.end();
	      found_one = 1;
	      
	  } else sitb++;
	  
        }
        if (!found_one) sita++;
	  
    }
    if (!addrA.empty())
    
        return false;
        if (!addrB.empty())
        {
	  return false;
        }
    
    
    
    // only check the resolvers if the resCompDepth > 0; 
    if (depthRestore > 0 )
    {
        // the resolvers are compared with up to resCompDepth -1 in the resolver network
        AgentID::setResCompDepth(depthRestore-1);
    // comparing resolvers
    std::vector<AgentID> agentsA = a.getResolvers();
    std::vector<AgentID> agentsB = b.getResolvers();
    std::vector<AgentID>::iterator ait = agentsA.begin();
    std::vector<AgentID>::iterator bit = agentsB.begin();
    
    while (ait != agentsA.end())
    {
        found_one = 0;
        bit = agentsB.begin();
        while (bit != agentsB.end())
        {
	  if ( (*ait) == (*bit)) 
	  {
	      agentsA.erase(ait);
	      ait = agentsA.begin();
	      agentsB.erase(bit);
	      bit = agentsB.end();
	      found_one = 1;
	      
	  } else bit++;
	  
        }
        if (!found_one) ait++;
	  
    }
    // restoring the comparison depth variable to the initial value; there must be no possible return statements between the 2 changes made to it
    AgentID::setResCompDepth(depthRestore);
    if (!agentsA.empty())
        return false;
    if (!agentsB.empty())
        return false;
    }
    
    // comparing userdefined parameters
    std::vector<UserdefParam> paramsA = a.getUserdefParams();
    std::vector<UserdefParam> paramsB = b.getUserdefParams();
    std::vector<UserdefParam>::iterator pita = paramsA.begin();
    std::vector<UserdefParam>::iterator pitb = paramsB.begin();
     
    while (pita != paramsA.end())
    {
        found_one = 0;
        pitb = paramsB.begin();
        while (pitb!=paramsB.end())
        {
	  if ( (*pita) == (*pitb)) 
	  {
	      
	      paramsA.erase(pita);
	      pita = paramsA.begin();
	      paramsB.erase(pitb);
	      pitb = paramsB.end();
	      found_one = 1;
	  }
	  else pitb++;
        }
	  if (!found_one) pita++;
	 
	  
    }
    if (!paramsA.empty())
        return false;
    if (!paramsB.empty())
        return false;
    
    return true;
    
}

bool operator== (UserdefParam &a,UserdefParam &b)
{
    if (a.getName().compare(b.getName()))
        return false;
    if (a.getValue().compare(b.getValue()))
        return false;
}

*/
}//end of acl namespace

}// end of fipa namespace



