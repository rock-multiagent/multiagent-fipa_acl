#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include "acl_message.h"
#include <boost/assign/list_of.hpp>
#include <stdexcept>

namespace fipa {

namespace acl {

std::map<ACLMessage::Performative, std::string> PerformativeTxt = boost::assign::map_list_of
	(ACLMessage::ACCEPT_PROPOSAL, "accept-proposal")
	(ACLMessage::AGREE, "agree")
	(ACLMessage::CANCEL, "cancel")
	(ACLMessage::CALL_FOR_PROPOSAL, "cfp")
	(ACLMessage::CONFIRM, "confirm")
	(ACLMessage::DISCONFIRM, "disconfirm")
	(ACLMessage::FAILURE, "failure")
	(ACLMessage::INFORM, "inform")
	(ACLMessage::INFORM_IF, "inform-if")
	(ACLMessage::INFORM_REF, "inform-ref")
	(ACLMessage::NOT_UNDERSTOOD, "not-understood")
	(ACLMessage::PROPAGATE, "propagate")
	(ACLMessage::PROPOSE, "propose")
	(ACLMessage::PROXY, "proxy")
	(ACLMessage::QUERY_IF, "query-if")
	(ACLMessage::QUERY_REF, "query-ref")
	(ACLMessage::REFUSE, "refuse")
	(ACLMessage::REJECT_PROPOSAL, "reject-proposal")
	(ACLMessage::REQUEST, "request")
	(ACLMessage::REQUEST_WHEN, "request-when")
	(ACLMessage::REQUEST_WHENEVER, "request-whenever")
	(ACLMessage::SUBSCRIBE, "subscribe");
    
const std::string illegalWordChars = std::string("() ") + char(0x00);
const std::string illegalWordStart = std::string("@#-0123456789"); 

void ACLMessage::initializeObject()
{
    receivers.clear();
    reply_to.clear();
    performative = PerformativeTxt[INFORM];
    language = std::string();
    encoding = std::string();
    ontology = std::string();
    protocol = std::string();
    conversation_id = std::string();
    reply_with = std::string();
    in_reply_to = std::string();
    reply_by = -1;
    reply_by1 = std::string();
    content = std::string();
    
    params.clear();        
}

ACLMessage::~ACLMessage()
{
    params.clear();
    reply_to.clear();
    receivers.clear();
}

ACLMessage::ACLMessage()
{
    initializeObject();
}

ACLMessage::ACLMessage(Performative perf)
{
    initializeObject();
    performative = PerformativeTxt[perf];
}


ACLMessage::ACLMessage(const std::string& perf) 
{
    initializeObject(); 
    
    if ( (perf.find_first_of(illegalWordChars) != std::string::npos) || (illegalWordStart.find_first_of(perf.c_str()[0]) != std::string::npos) )
        performative.clear();
    else performative = perf;
}

void ACLMessage::setPerformative(Performative perf)
{
    std::map<Performative, std::string>::iterator it;
    it = PerformativeTxt.find(perf);
    if(it == PerformativeTxt.end())
    {
	throw std::runtime_error("Cannot set performative. Performative unknown");
    }

    performative = it->second;
}

void ACLMessage::setPerformative(const std::string& str) 
{
    if ( (str.find_first_of(illegalWordChars) != std::string::npos) || (illegalWordStart.find_first_of(str.c_str()[0]) != std::string::npos) )
    {
	throw std::runtime_error("Cannot set performative. Performative contains illegal characters");
    }

    performative = str; 
}

std::string ACLMessage::getPerformative() const {return performative; }

void ACLMessage::addReceiver(const AgentID& aid) 
{
    if ( find(receivers.begin(),receivers.end(),aid) == receivers.end() )// prevent entering duplicates
							// NOTE: this function searches for and uses the overloaded == op not the resDepthEq()
        receivers.insert(receivers.begin(),aid); 
}

void ACLMessage::deleteReceiver(const AgentID& aid) 
{
    std::vector<AgentID>::iterator it;
    if ( (it = find(receivers.begin(),receivers.end(),aid)) != receivers.end() )// prevent entering duplicates
							// NOTE: this function searches for and uses the overloaded == op not the resDepthEq()
        receivers.erase(it);
}

void ACLMessage::clearReceivers() 
{
    receivers.clear();
}

std::vector<AgentID> ACLMessage::getAllReceivers() const {return receivers; }

void ACLMessage::addReplyTo(const AgentID& aid) 
{
    if ( find(reply_to.begin(),reply_to.end(),aid) == reply_to.end() )// prevent entering duplicates
							// NOTE: this function searches for and uses the overloaded == op not the resDepthEq()
        reply_to.insert(reply_to.begin(),aid); 
}

void ACLMessage::deleteReplyTo(const AgentID& aid) 
{
    std::vector<AgentID>::iterator it;
    if ( (it = find(reply_to.begin(),reply_to.end(),aid)) != reply_to.end() )// prevent entering duplicates
							// NOTE: this function searches for and uses the overloaded == op not the resDepthEq()
        reply_to.erase(it);
}

void ACLMessage::clearReplyTo()
{
    reply_to.clear(); 
}

std::vector<AgentID> ACLMessage::getAllReplyTo() const {return reply_to; }

void ACLMessage::setReplyBy(const long by) {reply_by = by; }

long ACLMessage::getReplyBy() const {return reply_by; }

void ACLMessage::setInReplyTo(const std::string& str) {in_reply_to = str; }

std::string ACLMessage::getInReplyTo() const {return in_reply_to; }

void ACLMessage::setReplyWith(const std::string& str) {reply_with = str; }

std::string ACLMessage::getReplyWith() const {return reply_with; }

void ACLMessage::setConversationID(const std::string& str) {conversation_id = str; }

std::string ACLMessage::getConversationID() const {return conversation_id; }

int ACLMessage::setProtocol(const std::string& str) 
{
    if ( (str.find_first_of(illegalWordChars) != std::string::npos) || (illegalWordStart.find_first_of(str.c_str()[0]) != std::string::npos) )
        return 1;

    protocol = str; 
    return 0;
}

std::string ACLMessage::getProtocol() const {return protocol; }

void ACLMessage::setOntology(const std::string& str) {ontology = str; }

std::string ACLMessage::getOntology() const {return ontology; }

void ACLMessage::setEncoding(const std::string& str) {encoding = str; }

std::string ACLMessage::getEncoding() const {return encoding; }

void ACLMessage::setLanguage(const std::string& str) {language = str; }

std::string ACLMessage::getLanguage() const {return language; }

void ACLMessage::setContent(const std::string& cont) {content = cont; }

std::string* ACLMessage::getContentPtr() { return &content; }

std::string ACLMessage::getContent() const {return content; }

void ACLMessage::setSender(const AgentID& sender1) 
{
    sender = sender1; 
}


AgentID ACLMessage::getSender() const 
{
    return sender; 
    
} 

void ACLMessage::addUserdefParam(const UserdefParam& p) 
{
    if (find (params.begin(),params.end(),p) == params.end() )
    params.insert(params.begin(),p);
}

std::vector<UserdefParam> ACLMessage:: getUserdefParams() const {return params;}

void ACLMessage::setUserdefParams(const std::vector<UserdefParam>& p) 
{
    params.clear();
    params.insert(params.begin(),p.begin(),p.end());
}


std::string ACLMessage::getReplyBy1(int formated) const
{
    if (formated == 0)
        return reply_by1;

    if (reply_by1.size() < 17)
        return reply_by1;

    std::string trim;
    trim += reply_by1.substr(0,4) + '-' + reply_by1.substr(4,2) + '-' + reply_by1.substr(6,2) + 'T' +
	  reply_by1.substr(8,2) + ':' + reply_by1.substr(10,2) + ':' + reply_by1.substr(12,2) + ':' + reply_by1.substr(14,3);	
    
    return trim;
}

void ACLMessage::_setReplyBy1(const std::string& date1)
{
    reply_by1 = date1;
}

int ACLMessage::setReplyBy1(const std::string& date1) 
{
    std::string trim,aux;
    
    if ((date1.size() < 10) || (date1.size() > 23)) {return 1;}
    aux.assign(date1.substr(0,4));

    if (aux.find_first_not_of(std::string("0123456789")) != std::string::npos) { return 2;} //checking year
    trim.append(aux);
    
    aux.assign(date1.substr(5,2));
    if (aux.find_first_not_of(std::string("0123456789")) != std::string::npos) { return 2;} //checking month
    trim.append(aux);
    
    aux.assign(date1.substr(8,2));
    if (aux.find_first_not_of(std::string("0123456789")) != std::string::npos) { return 2;} //checking day
    trim.append(aux);
    
    if (date1.size() >= 13)
    {
        aux.assign(date1.substr(11,2));
        if (aux.find_first_not_of(std::string("0123456789")) != std::string::npos) { return 2;} //checking hour
        trim.append(aux);

        if (date1.size() >= 16) 
        {
	  aux = date1.substr(14,2);
	  if (aux.find_first_not_of(std::string("0123456789")) != std::string::npos) { return 2;} //checking minutes
	  trim.append(aux);

	  if (date1.size() >= 19)
	  {
	      aux = date1.substr(17,2);
	      
	      if (aux.find_first_not_of(std::string("0123456789")) != std::string::npos) { return 2;} //checking seconds
	      trim.append(aux);

	      if (date1.size() == 23)
	      {
		aux = date1.substr(20,3);
		if (aux.find_first_not_of(std::string("0123456789")) != std::string::npos) { return 2;} //checking miliseconds
		trim.append(aux);
		
	      } else {
                  trim.append(std::string("000"));
              } // if (date1.size() == 23)
	      
	  } else {
              trim.append(std::string("00000"));
	  } // date1.size() == 19
	  
        } else {
	    trim.append(std::string("0000000"));
	} // date1.size >= 16
    } else {
	trim.append(std::string("000000000")); //fill the rest in with default if not specified
    } // date1.size >= 13
    
    reply_by1 = trim;
    return 0;
}


bool operator== (const ACLMessage& a,const ACLMessage& b)
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
    //if (a.getReplyBy1().compare(b.getReplyBy1()))
        //return false;
        
    /// comapring the reply_by1 parameters without the milisecons, as they are not yet supported by the parser    
    std::string repa, repb;
    if (repa.substr(0,repa.size()-3).compare(repb.substr(0,repb.size()-3)) )
        return false;
    
    if (!(a.getSender() == b.getSender()) )
        return false;
    
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
	  if ( resDepthEqual((*ait),(*bit),AgentID::resCompDepth)) 
	  {
	      agentsA.erase(ait);
	      ait = agentsA.begin();
	      agentsB.erase(bit);
	      bit = agentsB.end();
	      found_one = 1;
	      
	  } else {
	     ++bit;
	  }
        }

        if (!found_one) 
	{
	   ++ait;
	}
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
	  if ( resDepthEqual((*ait),(*bit),AgentID::resCompDepth)) 
	  {
	      agentsA.erase(ait);
	      ait = agentsA.begin();
	      agentsB.erase(bit);
	      bit = agentsB.end();
	      found_one = 1;
	      
	  } else {
              ++bit;
          }
	  
        }
        if (!found_one)
            ++ait;
	  
    }
    
    if (!agentsA.empty())
    { return false; }
    if (!agentsB.empty())
    { return false; }
    
    
    std::vector<UserdefParam> paramsA = a.getUserdefParams();
    std::vector<UserdefParam> paramsB = b.getUserdefParams();
    std::vector<UserdefParam>::iterator pita = paramsA.begin();
    std::vector<UserdefParam>::iterator pitb = paramsB.begin();
    
    while (pita != paramsA.end())
    {
        found_one = 0;
        pitb = paramsB.begin();
        while (pitb != paramsB.end())
        {
	    if (*pita == *pitb) 
	    {
	        paramsA.erase(pita);
	        pita = paramsA.begin();
	        paramsB.erase(pitb);
	        pitb = paramsB.end();
	        found_one = 1;
	        
	    } else {
                pitb++;
	    }
        }

        if (!found_one)
            pita++;
    }
    
    if (!paramsA.empty())
        return false;

    if (!paramsB.empty())
        return false;
   
    return true;
}





ACLMessage::ACLMessage(const ACLMessage& mes)
{
    initializeObject();
    if (!mes.getPerformative().empty()) performative = mes.getPerformative();
    if (!mes.getLanguage().empty()) language = mes.getLanguage();
    if (!mes.getEncoding().empty()) encoding = mes.getEncoding();
    if (!mes.getOntology().empty()) ontology = mes.getOntology();
    if (!mes.getProtocol().empty()) protocol = mes.getProtocol();
    if (!mes.getConversationID().empty()) conversation_id = mes.getConversationID();
    if (!mes.getReplyWith().empty()) reply_with = mes.getReplyWith();
    if (!mes.getInReplyTo().empty()) in_reply_to = mes.getInReplyTo();
    if (!mes.getReplyBy1().empty()) reply_by1 = mes.getReplyBy1();
    if (!mes.getContent().empty()) content = mes.getContent();
    
    
    AgentID temp;
    temp = mes.getSender();
    sender = temp;
	      
	 
    if (!mes.getAllReceivers().empty()) 
    {
        
        std::vector<AgentID> mesrec = mes.getAllReceivers();
        std::vector<AgentID>::iterator recit= mesrec.begin();
        for (; recit != mesrec.end();++recit)
        {
	  AgentID temp;
	  temp = (*recit);
	  receivers.insert(receivers.begin(),temp);
        }
    }
    if (!mes.getAllReplyTo().empty()) 
    {
        
        std::vector<AgentID> mesrec = mes.getAllReplyTo();
        std::vector<AgentID>::iterator recit= mesrec.begin();
        for (; recit != mesrec.end();++recit)
        {
	  AgentID temp;
	  temp = (*recit);
	  reply_to.insert(reply_to.begin(),temp);
        }
    }
    
    if (!mes.getUserdefParams().empty())
    {
        std::vector<UserdefParam> mesparams = mes.getUserdefParams();
        std::vector<UserdefParam>::iterator paramit = mesparams.begin();
    
        for (; paramit != mesparams.end();++paramit)
        {
	  UserdefParam temp2;
	  temp2 = (*paramit);
	  params.insert(params.begin(),temp2);
        }
    }  
        
}



ACLMessage& ACLMessage::operator=(const ACLMessage& mes)
{
    // checking against message1 = message1 case
    if (this == &mes)
	return *this;

    //freeing any previously filled in values for the userdefined parameters, reply_to, receivers and sender fields
    
    if (!params.empty())
    {
       params.clear();
    }
   
    if (!reply_to.empty())
    {
        reply_to.clear();
    }
    
    if (!receivers.empty())
    {
       receivers.clear();
    }
       
     //building the copied message
    initializeObject();	
    if (!mes.getPerformative().empty()) performative = mes.getPerformative();
    if (!mes.getLanguage().empty()) language = mes.getLanguage();
    if (!mes.getEncoding().empty()) encoding = mes.getEncoding();
    if (!mes.getOntology().empty()) ontology = mes.getOntology();
    if (!mes.getProtocol().empty()) protocol = mes.getProtocol();
    if (!mes.getConversationID().empty()) conversation_id = mes.getConversationID();
    if (!mes.getReplyWith().empty()) reply_with = mes.getReplyWith();
    if (!mes.getInReplyTo().empty()) in_reply_to = mes.getInReplyTo();
    if (!mes.getReplyBy1().empty()) reply_by1 = mes.getReplyBy1();
    if (!mes.getContent().empty()) content = mes.getContent();
        
       
    AgentID temp; 
    temp = mes.getSender();
    sender = temp;
	      
	  
    if (!mes.getAllReceivers().empty()) 
    {
        std::vector<AgentID> mesrec = mes.getAllReceivers();
        std::vector<AgentID>::iterator recit= mesrec.begin();
        for (; recit != mesrec.end();++recit)
        {
            AgentID temp;
            temp = (*recit);
            receivers.insert(receivers.begin(),temp);
        }
    }

    if (!mes.getAllReplyTo().empty()) 
    {
        std::vector<AgentID> mesrec = mes.getAllReplyTo();
        std::vector<AgentID>::iterator recit= mesrec.begin();
        for (; recit != mesrec.end();++recit)
        {
            AgentID temp;
            temp = (*recit);
            reply_to.insert(reply_to.begin(),temp);
        }
    }
        
    if (!mes.getUserdefParams().empty())
    {
        std::vector<UserdefParam> mesparams = mes.getUserdefParams();
        std::vector<UserdefParam>::iterator paramit = mesparams.begin();
    
        for (; paramit != mesparams.end();++paramit)
        {
            UserdefParam temp2;
            temp2 = (*paramit);
            params.insert(params.begin(),temp2);
        }
    }  
    
    return *this;
}


}//end of acl namespace

}// end of fipa namespace
