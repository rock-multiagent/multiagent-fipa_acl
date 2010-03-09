#include<iostream>
#include<sstream>
#include<string>
#include<cstring>
#include "ACLMessageOutputParser.h"
#include "ACLMessage.h"
#include "UserdefParam.h"
#include "AgentAID.h"

ACLMessageOutputParser::ACLMessageOutputParser()
{
                  useCodeTables = 0;
                  updateCodeTables = 1;
                  version = "1.0";
                  res_depth = 0;                              
}

char* ACLMessageOutputParser::getBitMessage()
{
      std::string mes;
      mes = getBitHeader() + getBitMessageType() + getBitMessageParameters() + getBitEndOfColl();
      
      char* output = new char(mes.length()+1);
      strcpy(output,mes.c_str());
      return output;
}

char ACLMessageOutputParser::getBitEndOfColl()
{
     return char(0x01); 
}

std::string ACLMessageOutputParser::getBitHeader()
{
     char p[3];
     p[2] = '\0'; p[0] = getBitMessageID(); p[1] = getBitMessageVersion();  
     return std::string(p);
}

char ACLMessageOutputParser::getBitMessageID()
{
     if (useCodeTables == 0) return 0xfa;
     if (updateCodeTables == 1) return 0xfb;
     return 0xfc;
}
char ACLMessageOutputParser::getBitMessageVersion()
{
     return ((int)version[0]-48)*16 + ((int)version[2]-48);
}
std::string ACLMessageOutputParser::getBitMessageType()
{
            for (int i = 0; i < 21; i++)
                if (ACLMessage::perfs[i].compare((*msg).getPerformative()))
                              { 
                                char a[2]; a[0] = char(i+1); a[1] = '\0'; 
                                return std::string(a);
                              }
            return char(0x00) + getBitBinWord((*msg).getPerformative());
} 

std::string ACLMessageOutputParser::getBitBinWord(std::string sword)
{
            if (!useCodeTables) return char(0x10) + sword + char(0x00);
           // return char(0x11) + getCTIndex(word);
}
std::string ACLMessageOutputParser::getBitMessageParameters()
{
            return getBitPredefMessageParams() + getBitUserdefMessageParams();
}

std::string ACLMessageOutputParser::getBitPredefMessageParams()
{
            std::string retstr = std::string("\0");
            if ((*msg).getSender() != NULL) retstr = retstr + char(0x02) + getBitAID((*msg).getSender(), res_depth);
            if ((*msg).getAllReceivers() != NULL) retstr =retstr + char(0x03) + getBitAIDColl((*msg).getAllReceivers(),res_depth);
            if ((*msg).getContent().compare("\0")) retstr = retstr + char(0x04) + getBitBinString((*msg).getContent());
            if ((*msg).getReplyWith().compare("\0")) retstr = retstr + char(0x05) + getBitBinExpression((*msg).getReplyWith(),'s');
            if ((*msg).getReplyBy() != -1) retstr = retstr + char(0x06) + getBitBinDateTimeToken((*msg).getReplyBy1());
            if ((*msg).getInReplyTo().compare("\0")) retstr = retstr + char(0x07) + getBitBinExpression((*msg).getInReplyTo(),'s');
            if ((*msg).getAllReplyTo() != NULL) retstr = retstr + char(0x08) + getBitAIDColl((*msg).getAllReplyTo(),res_depth);
            if ((*msg).getLanguage().compare("\0")) retstr = retstr + char(0x09) + getBitBinExpression((*msg).getLanguage(),'s');
            if ((*msg).getEncoding().compare("\0")) retstr = retstr + char(0x0a) + getBitBinExpression((*msg).getEncoding(),'s');
            if ((*msg).getOntology().compare("\0")) retstr = retstr + char(0x0b) + getBitBinExpression((*msg).getOntology(),'s');
            if ((*msg).getProtocol().compare("\0")) retstr = retstr + char(0x0c) + getBitBinExpression((*msg).getProtocol(),'w');
            if ((*msg).getConversationID().compare("\0")) retstr = retstr + char(0x0d) + getBitBinExpression((*msg).getConversationID(),'s');
            
            return retstr;
}
std::string ACLMessageOutputParser::getBitUserdefMessageParams()
{
            std::string retstr = std::string("\0");
            std::set<UserdefParam*>* s = (*msg).getUserdefParams();
            std::set<UserdefParam*>::iterator it = (*s).begin();
            for (it; it != (*s).end(); it++)
                retstr = retstr + char(0x00) + bitParseParam(*it);
            return retstr;
}
std::string ACLMessageOutputParser::getBitUserdefParams(std::set<UserdefParam*>* params)
{
            std::string retstr = std::string("\0");
            std::set<UserdefParam*>::iterator it = (*params).begin();
            for (it; it != (*params).end(); it++)
                retstr = retstr + char(0x04) + bitParseParam(*it);
            return retstr;
}
std::string ACLMessageOutputParser::bitParseParam(UserdefParam* p)
{
           return getBitBinWord((*p).getName()) + getBitBinExpression((*p).getValue(),'s');
}


std::string ACLMessageOutputParser::getBitAID(AgentAID* aid, int depth)
{
            if (depth > 0)
            return char(0x02) + getBitBinWord((*aid).getName())+
            (((*aid).getAdresses() == NULL)? "\0" : char(0x02) + getBinURLCol((*aid).getAdresses())) +
            (((*aid).getResolvers() == NULL)? "\0" : getBitAIDColl((*aid).getResolvers(),depth-1)) +
            (((*aid).getUserdefParams() == NULL)? "\0" : getBitUserdefParams((*aid).getUserdefParams())) +
            getBitEndOfColl();
             
             /*  return char(0x02) + getBitBinWord((*aid).getName()) + char(0x02) + getBinURLCol((*aid).getAdresses())
               + getBitAIDColl((*aid).getResolvers(),depth-1) + getBitUserdefParams((*aid).getUserdefParams()) + getBitEndOfColl();
            */
            
           return char(0x02) + getBitBinWord((*aid).getName())+
            (((*aid).getAdresses() == NULL)? "\0" : char(0x02) + getBinURLCol((*aid).getAdresses())) +
            
            (((*aid).getUserdefParams() == NULL)? "\0" : getBitUserdefParams((*aid).getUserdefParams())) +
            getBitEndOfColl();
}
std::string ACLMessageOutputParser::getBinURLCol(std::set<std::string>* adrr)
{
            std::string retstr = std::string("\0");
            std::set<std::string>::iterator it = (*adrr).begin();
            for (it; it != (*adrr).end(); it++)
                retstr = retstr + getBitBinWord(*it);
            retstr = retstr + getBitEndOfColl();
            
            return retstr;
}
std::string ACLMessageOutputParser::getBitResolvers(std::set<AgentAID*>* aids,int depth)
{
            return char(0x03) + getBitAIDColl(aids,depth);
}

std::string ACLMessageOutputParser::getBitAIDColl(std::set<AgentAID*>* aids, int depth)
{
            std::string retstr = std::string("\0");
            std::set<AgentAID*>::iterator it = (*aids).begin();
            for (it; it != (*aids).end(); it++)
                retstr = retstr + getBitAID(*it, depth);
            return retstr + getBitEndOfColl();
}

std::string ACLMessageOutputParser::getBitBinExpression(std::string sword,char c)
{
            if (sword.compare((*msg).getContent())) return char(0xff) + getBitBinString(sword,0);
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
            if (!useCodeTables) return char(0x14) + sword + char(0x00);
            // char(0x15) + return getCTIndex(sword);
}
std::string ACLMessageOutputParser::getBitBinString(std::string sword,int codeTables)
{
            if (!codeTables) return char(0x14) + sword + char(0x00);
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
            std::string retstr = "\0";
            int i;
            for (i = 0; i < date1.length(); i = i + 2)
            {
                
                retstr = retstr + getBitCodedNumber(date1.substr(i,2));
            }
            
            return retstr;

}
std::string ACLMessageOutputParser::getBitCodedNumber(std::string cn)
{
            std::string retstr = "\0";
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
