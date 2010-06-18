#include <iostream>
#include <vector>
#include <string>
#include "ACLMessage.h"

namespace fipa {

namespace acl {

    
    
const std::string ACLMessage::perfs[22] = {"accept-proposal","agree","cancel","cfp","confirm","disconfirm","failure","inform",
				   "inform-if","inform-ref","not-understood","propagate","propose","proxy","query-if",
				   "query-ref","refuse","reject-proposal","request","request-when","request-whenever","subscribe"};




  
void ACLMessage::initializeObject()
{
	
         receivers.clear();
         reply_to.clear();
	/*
         language = std::string();
         language.clear();
         encoding = std::string();
         encoding.clear();
         ontology = std::string();
         ontology.clear();
         protocol = std::string();
         protocol.clear();
         conversation_id = std::string();
         conversation_id.clear();
         reply_with = std::string();
         reply_with.clear();
         in_reply_to = std::string();
         in_reply_to.clear();
         reply_by = -1;
         reply_by1 = std::string();
         reply_by1.clear();
         content = std::string();
         content.clear();
         */
         
         params.clear();        
         
}

ACLMessage::~ACLMessage()
{
    params.clear();
    reply_to.clear();
    receivers.clear();
    /*
    if (sender) delete sender;
    if (receivers)
    {
    std::vector<AgentAID*>::iterator it = receivers->begin();
    for (it; it!=receivers->end(); it++)
        delete (*it);
    receivers->clear();
    delete receivers;
    }
    
    if (reply_to)
    {
    std::vector<AgentAID*>::iterator it = reply_to->begin();
    for (it; it != reply_to->end(); it++)
        delete (*it);
    reply_to->clear();
    delete reply_to;
    }
    if (params)
    {
    std::vector<UserdefParam*>::iterator it2 = params->begin();
    for (it2; it2 != params->end(); it2++)
        delete (*it2);
    params->clear();
    delete params;
    }
    */
}

ACLMessage::ACLMessage()
{
	initializeObject();
}

ACLMessage::ACLMessage(predefinedPerformatives perf)

{
	initializeObject();
	performative = ACLMessage::perfs[perf];
}

ACLMessage::ACLMessage(std::string perf) {initializeObject(); performative = perf; }

void ACLMessage::setPerformative(std::string str) {performative = str; }

std::string ACLMessage::getPerformative() const {return performative; }

void ACLMessage::addReceiver(AgentAID &aid) 
{
    receivers.insert(receivers.begin(),aid); 
}

void ACLMessage::deleteReceiver(AgentAID &aid) 
{
    std::vector<AgentAID>::iterator it = receivers.begin();
    for (it; it != receivers.end(); it++)
       {
        AgentAID a = *it;
        if ( a == aid)
	  receivers.erase(it);
    }
}

void ACLMessage::clearReceivers() 
{
    receivers.clear();
}

std::vector<AgentAID> ACLMessage::getAllReceivers() const {return receivers; }

void ACLMessage::addReplyTo(AgentAID &aid) 
{
    reply_to.insert(reply_to.begin(),aid); 
}

void ACLMessage::deleteReplyTo(AgentAID aid) 
{
    std::vector<AgentAID>::iterator it = reply_to.begin();
    for (it; it != reply_to.end(); it++)
    {
        AgentAID a = *it;
        if ( a == aid)
	  reply_to.erase(it);
    }
}

void ACLMessage::clearReplyTo() 
{
    reply_to.clear(); 
}

std::vector<AgentAID> ACLMessage::getAllReplyTo() const {return reply_to; }

void ACLMessage::setReplyBy(long by) {reply_by = by; }

long ACLMessage::getReplyBy() const {return reply_by; }

void ACLMessage::setInReplyTo(std::string str) {in_reply_to = str; }

std::string ACLMessage::getInReplyTo() const {return in_reply_to; }

void ACLMessage::setReplyWith(std::string str) {reply_with = str; }

std::string ACLMessage::getReplyWith() const {return reply_with; }

void ACLMessage::setConversationID(std::string str) {conversation_id = str; }

std::string ACLMessage::getConversationID() const {return conversation_id; }

void ACLMessage::setProtocol(std::string str) {protocol = str; }

std::string ACLMessage::getProtocol() const {return protocol; }

void ACLMessage::setOntology(std::string str) {ontology = str; }

std::string ACLMessage::getOntology() const {return ontology; }

void ACLMessage::setEncoding(std::string str) {encoding = str; }

std::string ACLMessage::getEncoding() const {return encoding; }

void ACLMessage::setLanguage(std::string str) {language = str; }

std::string ACLMessage::getLanguage() const {return language; }

void ACLMessage::setContent(std::string cont) {content = cont; }

std::string ACLMessage::getContent() const {return content; }

void ACLMessage::setSender(AgentAID &sender1) 
{
    sender = sender1; 
}


AgentAID ACLMessage::getSender() const 
{
    return sender; 
    
} 

void ACLMessage::addUserdefParam(UserdefParam &p) 
{
    params.insert(params.begin(),p);
}

std::vector<UserdefParam> ACLMessage:: getUserdefParams() const {return params;}

void ACLMessage::setUserdefParams(std::vector<UserdefParam> p) 
{
    params.clear();
    params.insert(params.begin(),p.begin(),p.end());
}


std::string ACLMessage::getReplyBy1() const {return reply_by1;}

void ACLMessage::setReplyBy1(std::string date1) {reply_by1 = date1;}

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
    std::vector<AgentAID> agentsA = a.getAllReceivers();
    std::vector<AgentAID> agentsB = b.getAllReceivers();
    std::vector<AgentAID>::iterator ait = agentsA.begin();
    std::vector<AgentAID>::iterator bit = agentsB.begin();
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
*/




ACLMessage::ACLMessage(const ACLMessage &mes)
{
    initializeObject();
    if (!mes.getLanguage().empty()) language = mes.getLanguage();
    if (!mes.getEncoding().empty()) encoding = mes.getEncoding();
    if (!mes.getOntology().empty()) ontology = mes.getOntology();
    if (!mes.getProtocol().empty()) protocol = mes.getProtocol();
    if (!mes.getConversationID().empty()) conversation_id = mes.getConversationID();
    if (!mes.getReplyWith().empty()) reply_with = mes.getReplyWith();
    if (!mes.getInReplyTo().empty()) in_reply_to = mes.getInReplyTo();
    if (!mes.getReplyBy1().empty()) reply_by1 = mes.getReplyBy1();
    if (!mes.getContent().empty()) content = mes.getContent();
    
    
	      AgentAID temp = AgentAID();
	      temp = mes.getSender();
	      sender = temp;
	      
	 
    if (!mes.getAllReceivers().empty()) 
    {
        
        std::vector<AgentAID> mesrec = mes.getAllReceivers();
        std::vector<AgentAID>::iterator recit= mesrec.begin();
        for (recit; recit != mesrec.end(); recit++)
        {
	  AgentAID temp = AgentAID();
	  temp = (*recit);
	  receivers.insert(receivers.begin(),temp);
        }
    }
    if (!mes.getAllReplyTo().empty()) 
    {
        
        std::vector<AgentAID> mesrec = mes.getAllReplyTo();
        std::vector<AgentAID>::iterator recit= mesrec.begin();
        for (recit; recit != mesrec.end(); recit++)
        {
	  AgentAID temp = AgentAID();
	  temp = (*recit);
	  reply_to.insert(reply_to.begin(),temp);
        }
    }
    
    if (!mes.getUserdefParams().empty())
    {
        std::vector<UserdefParam> mesparams = mes.getUserdefParams();
        std::vector<UserdefParam>::iterator paramit = mesparams.begin();
    
        for (paramit; paramit != mesparams.end(); paramit++)
        {
	  UserdefParam temp2 = UserdefParam();
	  temp2 = (*paramit);
	  params.insert(params.begin(),temp2);
        }
    }  
        
}



ACLMessage& ACLMessage::operator=(ACLMessage &mes)
{
    // checking against message1 = message1 case
    if (this != &mes)
    {    
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
        
       
	      AgentAID temp = AgentAID();
	      temp = mes.getSender();
	      sender = temp;
	      
	  
        if (!mes.getAllReceivers().empty()) 
        {
	  
	  std::vector<AgentAID> mesrec = mes.getAllReceivers();
	  std::vector<AgentAID>::iterator recit= mesrec.begin();
	  for (recit; recit != mesrec.end(); recit++)
	  {
	      AgentAID temp = AgentAID();
	      temp = (*recit);
	      receivers.insert(receivers.begin(),temp);
	  }
        }
        if (!mes.getAllReplyTo().empty()) 
        {
	  
	  std::vector<AgentAID> mesrec = mes.getAllReplyTo();
	  std::vector<AgentAID>::iterator recit= mesrec.begin();
	  for (recit; recit != mesrec.end(); recit++)
	  {
	      AgentAID temp = AgentAID();
	      temp = (*recit);
	      reply_to.insert(reply_to.begin(),temp);
	  }
        }
        
        if (!mes.getUserdefParams().empty())
        {
	  std::vector<UserdefParam> mesparams = mes.getUserdefParams();
	  std::vector<UserdefParam>::iterator paramit = mesparams.begin();
        
	  for (paramit; paramit != mesparams.end(); paramit++)
	  {
	      UserdefParam temp2 = UserdefParam();
	      temp2 = (*paramit);
	      params.insert(params.begin(),temp2);
	  }
        }  
    }
    
    return *this;
}


}//end of acl namespace

}// end of fipa namespace
