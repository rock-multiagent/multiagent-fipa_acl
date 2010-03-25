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
#include "AgentAID.h"

namespace fipa {

namespace acl {

/*
	both constructors initialize the object fields
	they can be checked and changed later by setter and geter methods
*/

void ACLMessageOutputParser::setMessage(ACLMessage* a)
{
	msg = a;
  		  useCodeTables = 0;
                  updateCodeTables = 1;
                  version = "1.0";
                  res_depth = 0; 
}


ACLMessageOutputParser::ACLMessageOutputParser()
{
                  useCodeTables = 0;
                  updateCodeTables = 1;
                  version = "1.0";
                  res_depth = 0; 
			                   
}

/*
	prints the parsed message to a stream given as argument(as a string)
*/
int ACLMessageOutputParser::printParsedMessage(std::string stream)
{
     std::ofstream out(stream.c_str(), std::ios_base::binary);
	if (!out)
		return 0;
	
	std::string output = getBitMessage();
	

	for (int i = 0; i < output.length(); i++){
		out.put(output[i]);
	std::cout<<int(output[i])<<"  ";	
	}
	std::cout<<std::endl;

	out.close();
	return 1;
     
     
}

/*
	main function in the production tree, returns the encoded message as a sequence of bytes kept in a string object
*/
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
/*
	puts the header toghether
*/
std::string ACLMessageOutputParser::getBitHeader()
{	
	std::string retstr = std::string();
	retstr.clear();
	
     char id = getBitMessageID();
     char version = getBitMessageVersion();
 
	
     return retstr.append(1,id).append(1,version);
}
/*
	depending on the flags it sets the ID
*/
char ACLMessageOutputParser::getBitMessageID()
{	
	
     if (useCodeTables == 0) return 0xfa;
     if (updateCodeTables == 1) return 0xfb;
     return 0xfc;
}
/*
	gets the message version in one byte
	the message version has a very specific format: digit+dot+digit
*/
char ACLMessageOutputParser::getBitMessageVersion()
{
	     
	return char(((int)version[0]-48)*16 + ((int)version[2]-48));
}
/*
	gets the message performative;
	it compares the string of the message performative to all the predefined ones; if it is one of them it return accordingly if not it returns the 
custom performative
*/
std::string ACLMessageOutputParser::getBitMessageType()
{
            for (int i = 0; i < 22; i++)
                if (!ACLMessage::perfs[i].compare((*msg).getPerformative()))
                              { 
				
                                char a[2]; a[0] = char(i+1); a[1] = '\0'; 
                                return std::string(a);
                              }
		
            return (char(0x00) + getBitBinWord((*msg).getPerformative()));
} 
/*
	quite frequently used production;
	* currently it does not have complete functionality: the code tables need to be implemented in order to use them
*/
std::string ACLMessageOutputParser::getBitBinWord(std::string sword)
{
		if (useCodeTables == 0) return (char(0x10) + sword + char(0x00));
           // return char(0x11) + getCTIndex(sword);
}

std::string ACLMessageOutputParser::getBitMessageParameters()
{
	         
	return (getBitPredefMessageParams() + getBitUserdefMessageParams());
}
/*	
	checks all predefined message parameters whether they are set or not and it encodes them accordingly
*/
std::string ACLMessageOutputParser::getBitPredefMessageParams()
{
            std::string retstr = std::string();
	    retstr.clear();

            if ((*msg).getSender() != NULL) retstr = retstr + char(0x02) + getBitAID((*msg).getSender(), res_depth); 
            if (!(*msg).getAllReceivers()->empty()) retstr =retstr + char(0x03) + getBitAIDColl((*msg).getAllReceivers(),res_depth); 
            if ((*msg).getContent().compare("\0")) retstr = retstr + char(0x04) + getBitBinString((*msg).getContent()); 
            if ((*msg).getReplyWith().compare("\0")) retstr = retstr + char(0x05) + getBitBinExpression((*msg).getReplyWith(),'s'); 
            if ((*msg).getReplyBy1().compare("\0")) retstr = retstr + char(0x06) + getBitBinDateTimeToken((*msg).getReplyBy1()); 
            if ((*msg).getInReplyTo().compare("\0")) retstr = retstr + char(0x07) + getBitBinExpression((*msg).getInReplyTo(),'s'); 
            if (!(*msg).getAllReplyTo()->empty()) retstr = retstr + char(0x08) + getBitAIDColl((*msg).getAllReplyTo(),res_depth);
            if ((*msg).getLanguage().compare("\0")) retstr = retstr + char(0x09) + getBitBinExpression((*msg).getLanguage(),'s'); 
            if ((*msg).getEncoding().compare("\0")) retstr = retstr + char(0x0a) + getBitBinExpression((*msg).getEncoding(),'s');
            if ((*msg).getOntology().compare("\0")) retstr = retstr + char(0x0b) + getBitBinExpression((*msg).getOntology(),'s'); 
            if ((*msg).getProtocol().compare("\0")) retstr = retstr + char(0x0c) + getBitBinExpression((*msg).getProtocol(),'w');
            if ((*msg).getConversationID().compare("\0")) retstr = retstr + char(0x0d) + getBitBinExpression((*msg).getConversationID(),'s');
            
            return retstr;
}
/*
	parses all the user defined parameters of the message if any
*/
std::string ACLMessageOutputParser::getBitUserdefMessageParams()
{
            std::string retstr = std::string();
	    retstr.clear();

            std::set<UserdefParam*>* s = (*msg).getUserdefParams();
	    if(s->empty()) return retstr;
	
            std::set<UserdefParam*>::iterator it; 
	    it = (*s).begin();
            for (it; it != (*s).end(); it++)
		retstr = retstr + char(0x00) + bitParseParam(*it);

            return retstr;
}
/*
	parses a set of user defined parametrs;
	different from the getBitUserdefMessageParams() method in that there is more general; the former is to be called only for message parameters;
	difference was imposed by the specification	
*/
std::string ACLMessageOutputParser::getBitUserdefParams(std::set<UserdefParam*>* params)
{
            std::string retstr = std::string();
            std::set<UserdefParam*>::iterator it = (*params).begin();
            for (it; it != (*params).end(); it++)
                retstr = retstr + char(0x04) + bitParseParam(*it);

            return retstr;
}
/*
	parses one user defined parameter
	the specification does not differentiate at this level between message parameters and other kinds so this method is called by both
*/
std::string ACLMessageOutputParser::bitParseParam(UserdefParam* p)
{
		return getBitBinWord((*p).getName()) + getBitBinExpression((*p).getValue(),'s');
}

/*
	parses an AgentAID instance
	as some fields are optional the method first checks for their existance
	it also keeps record of the res_depth
*/
std::string ACLMessageOutputParser::getBitAID(AgentAID* aid, int depth)
{
            if (depth > 0)
            return char(0x02) + getBitBinWord((*aid).getName())+
            (( (*(*aid).getAdresses()).empty() == true)? "" : (char(0x02) + getBinURLCol((*aid).getAdresses()))) +
            (( (*(*aid).getResolvers()).empty() == true)? "" : getBitResolvers((*aid).getResolvers(),depth-1)) +
            (( (*(*aid).getUserdefParams()).empty() == true)? "" : getBitUserdefParams((*aid).getUserdefParams())) +
            getBitEndOfColl();
             
                         
           return char(0x02) + getBitBinWord((*aid).getName())+
            (( (*(*aid).getAdresses()).empty() == true)? "" : (char(0x02) + getBinURLCol((*aid).getAdresses()))) +
            
            (( (*(*aid).getUserdefParams()).empty() == true)? "" : getBitUserdefParams((*aid).getUserdefParams())) +
            getBitEndOfColl();
}

/*
	currently used to encode the adresses of the AgentAID instances
*/
std::string ACLMessageOutputParser::getBinURLCol(std::set<std::string>* adrr)
{
            std::string retstr = std::string();
            std::set<std::string>::iterator it = (*adrr).begin();
            for (it; it != (*adrr).end(); it++)
                retstr = retstr + getBitBinWord(*it);
            retstr = retstr + getBitEndOfColl();
            
            return retstr;
}

std::string ACLMessageOutputParser::getBitResolvers(std::set<AgentAID*>* aids,int depth)
{
            return (char(0x03) + getBitAIDColl(aids,depth));
}

/*
	parses a set of AgentAID instances
	the resolvers depth variable that is being passed around is not modified in this function
*/
std::string ACLMessageOutputParser::getBitAIDColl(std::set<AgentAID*>* aids, int depth)
{
            std::string retstr = std::string();
            std::set<AgentAID*>::iterator it = (*aids).begin();
            for (it; it != (*aids).end(); it++)
                retstr = retstr + getBitAID(*it, depth);
            return retstr + getBitEndOfColl();
}
/*
	implements the binary expression production of the grammar
	not complete(w.r.t. the specification) in functionality
	implementing the messages without the rest of the architecture makes it difficult to anticipate when and how some productions may/will be used so only a few of the productions were implemented and the char parameter was added to choose between them, as no other decission maker/constraint was identified
*/
std::string ACLMessageOutputParser::getBitBinExpression(std::string sword,char c)
{
            if (!sword.compare((*msg).getContent())) return char(0xff) + getBitBinString(sword,0);
            if (c == 's') return getBitBinString(sword);
            if (c == 'w') return getBitBinWord(sword);
}
/*
	overloaded version of the above function
*/
std::string ACLMessageOutputParser::getBitBinExpression(double n,char base)
{
            
            return getBitBinNumber(n,base);
}

/*
	parses a number according to the specification(see comment 9 of the specification)
	for ease it is turned into a string and passed on
*/
std::string ACLMessageOutputParser::getBitBinNumber(double n,char base)
{
            std::stringstream ss (std::stringstream::in | std::stringstream::out);
            ss << n;
            std::string test = ss.str();  
            if(base == 'h') return char(0x13) + getBitDigits(test);
            if(base == 'o' || base == 'd') return char(0x12) + getBitDigits(test);
}

/*
	implements the binary string production
	functionality not complete as code tables are not yet implemented
*/
std::string ACLMessageOutputParser::getBitBinString(std::string sword)
{
            if (!useCodeTables) return char(0x14) + sword + char(0x00);
            // char(0x15) + return getCTIndex(sword);
}

/*
	implements the binary string production
	functionality not complete as code tables are not yet implemented
	the second argument is an explicit option for codeTables(needed for the getBitBinExpression() )
*/
std::string ACLMessageOutputParser::getBitBinString(std::string sword,int codeTables)
{
            if (!codeTables) return char(0x14) + sword + char(0x00);
            // char(0x15) + return getCTIndex(sword);
}

std::string ACLMessageOutputParser::getBitDigits(std::string dig)
{
           return getBitCodedNumber(dig);
}
/*
	implements the date time token production of the grammar
	not complete(w.r.t. the specification) in functionality 
	implementing the messages without the rest of the architecture makes it difficult to anticipate when and how some productions may/will be used so only a few of the productions were implemented and the char parameter was added to choose between them, as no other decission maker/constraint was identified
*/
std::string ACLMessageOutputParser::getBitBinDateTimeToken(std::string date1)
{
            return char(0x20) + getBitBinDate(date1);
}

/*
	takes the string representing the date and passes it's digits 2 by 2(as length 2 sugstrings) to the byte encoding function
	it did not explicitly specify but it was clear from the way it was stated that the date is to be encoded as a coded number(comment 9 of the specification)
*/
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

/*
	implements a coded number passed as a string
	it goes through it digit by digit
*/
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

/*
	different version of the above function needed for parsing the date string
	because the date string is passed as substrings and then concatenated back toghether in the caller function the above function would not perform as desired(it adds a padding 0x00 byte after each substring)
*/
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

}//end of acl namespace

}// end of fipa namespace
