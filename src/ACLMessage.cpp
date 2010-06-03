#include <iostream>
#include <set>
#include <string>
#include "ACLMessage.h"

namespace fipa {

namespace acl {

    
    
const std::string ACLMessage::perfs[22] = {"accept-proposal","agree","cancel","cfp","confirm","disconfirm","failure","inform","inform-if","inform-ref","not-understood","propagate","propose","proxy","query-if","query-ref","refuse","reject-proposal","request","request-when","request-whenever","subscribe"};
/*
bool operator== (ACLMessage &a, ACLMessage &b)
{
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
    std::set<AgentAID*>* agentsA = a.getReceivers();
    std::set<AgentAID*>* agentsB = b.getReceivers();
    std::set<AgentAID*>::iterator ait = agentsA->begin();
    std::set<AgentAID*>::iterator bit = agentsB->begin();
    for (ait; ait != agentsA->end(); ait++)
    {
        for (bit; bit != agentsB->end(); bit++)
	  if ( (*(*ait)) == (*(*bit))) 
	  {
	      agentsA->erase(ait);
	      agentsB->erase(bit);
	      bit = agentsB->end();
	      
	  }
    }
    if (agentsA->begin() != agentsA->end())
        return false;
    if (agentsB->begin() != agentsB->end())
        return false;
    
    //checking if reply_to sets of the message are  the same
    agentsA = a.getReplyTo();
    agentsB = b.getReplyTo();
    ait = agentsA->begin();
    bit = agentsB->begin();
    for (ait; ait != agentsA->end(); ait++)
    {
        for (bit; bit != agentsB->end(); bit++)
	  if ( (*(*ait)) == (*(*bit))) 
	  {
	      agentsA->erase(ait);
	      agentsB->erase(bit);
	      bit = agentsB->end();
	      
	  }
    }
    if (agentsA->begin() != agentsA->end())
        return false;
    if (agentsB->begin() != agentsB->end())
        return false;
    
    
    std::set<UserdefParam*>* paramsA = a.getUserdefParams();
    std::set<UserdefParam*>* paramsB = b.getUserdefParams();
    std::set<UserdefParam*>::iterator pita = paramsA->begin();
    std::set<UserdefParam*>::iterator pitb = paramsB->begin();
    for (pita; pita != paramsA->end(); pita++)
    {
        for (pitb; pitb != paramsB->end(); pitb++)
	  if ( (*(*pita)) == (*(*pitb))) 
	  {
	      paramsA->erase(pita);
	      paramsB->erase(pitb);
	      pitb = paramsB->end();
	      
	  }
    }
    if (paramsA->begin() != paramsA->end())
        return false;
    if (paramsB->begin() != paramsB->end())
        return false;
    
    
    return true;
}
*/

ACLMessage::ACLMessage(ACLMessage &mes)
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
    
    if (mes.getSender() != NULL)
        {
	      AgentAID *temp = new AgentAID();
	      *temp = *(mes.getSender());
	      sender = temp;
	      
	  }
    if (!mes.getAllReceivers()->empty()) 
    {
        
        std::set<AgentAID*>* mesrec = mes.getAllReceivers();
        std::set<AgentAID*>::iterator recit= mesrec->begin();
        for (recit; recit != mesrec->end(); recit++)
        {
	  AgentAID *temp = new AgentAID();
	  *temp = (*(*recit));
	  receivers->insert(temp);
        }
    }
    if (!mes.getAllReplyTo()->empty()) 
    {
        
        std::set<AgentAID*>* mesrec = mes.getAllReplyTo();
        std::set<AgentAID*>::iterator recit= mesrec->begin();
        for (recit; recit != mesrec->end(); recit++)
        {
	  AgentAID *temp = new AgentAID();
	  *temp = (*(*recit));
	  reply_to->insert(temp);
        }
    }
    
    if (!mes.getUserdefParams()->empty())
    {
        std::set<UserdefParam*>* mesparams = mes.getUserdefParams();
        std::set<UserdefParam*>::iterator paramit = mesparams->begin();
    
        for (paramit; paramit != mesparams->end(); paramit++)
        {
	  UserdefParam *temp2 = new UserdefParam();
	  *temp2 = (*(*paramit));
	  params->insert(temp2);
        }
    }  
        
}
ACLMessage& ACLMessage::operator=(ACLMessage &mes)
{
    params->clear();
    reply_to->clear();
    receivers->clear();
    if (this != &mes)
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
        
        if (mes.getSender() != NULL)
	  {
	      AgentAID *temp = new AgentAID();
	      *temp = *(mes.getSender());
	      sender = temp;
	      
	  }
        if (!mes.getAllReceivers()->empty()) 
        {
	  
	  std::set<AgentAID*>* mesrec = mes.getAllReceivers();
	  std::set<AgentAID*>::iterator recit= mesrec->begin();
	  for (recit; recit != mesrec->end(); recit++)
	  {
	      AgentAID *temp = new AgentAID();
	      *temp = (*(*recit));
	      receivers->insert(temp);
	  }
        }
        if (!mes.getAllReplyTo()->empty()) 
        {
	  
	  std::set<AgentAID*>* mesrec = mes.getAllReplyTo();
	  std::set<AgentAID*>::iterator recit= mesrec->begin();
	  for (recit; recit != mesrec->end(); recit++)
	  {
	      AgentAID *temp = new AgentAID();
	      *temp = (*(*recit));
	      reply_to->insert(temp);
	  }
        }
        
        if (!mes.getUserdefParams()->empty())
        {
	  std::set<UserdefParam*>* mesparams = mes.getUserdefParams();
	  std::set<UserdefParam*>::iterator paramit = mesparams->begin();
        
	  for (paramit; paramit != mesparams->end(); paramit++)
	  {
	      UserdefParam *temp2 = new UserdefParam();
	      *temp2 = (*(*paramit));
	      params->insert(temp2);
	  }
        }  
    }
    
    return *this;
}
  
void ACLMessage::initializeObject()
{
	
	//sender = new AgentAID();
	sender = NULL;
        receivers = new std::set<AgentAID*>();
	if (!(*receivers).empty()) (*receivers).clear();
        reply_to = new std::set<AgentAID*>();
	if (!(*reply_to).empty()) (*reply_to).clear();
	
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
         
        params = new std::set<UserdefParam*>();
		
	if (!(*params).empty()) (*params).clear();
		
        
         content = std::string();
         content.clear();
}

ACLMessage::~ACLMessage()
{
    delete sender;
    receivers->clear();
    delete receivers;
    reply_to->clear();
    delete reply_to;
    params->clear();
    delete params;
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

ACLMessage::ACLMessage(std::string perf) {performative = perf; initializeObject();}

void ACLMessage::setPerformative(std::string str) {performative = str; }

std::string ACLMessage::getPerformative() {return performative; }

void ACLMessage::addReceiver(AgentAID* aid) {(*receivers).insert(aid); }

void ACLMessage::deleteReceiver(AgentAID* aid) {(*receivers).erase(aid); }

void ACLMessage::clearReceivers() {(*receivers).clear(); }

std::set<AgentAID*>* ACLMessage::getAllReceivers() {return receivers; }

void ACLMessage::addReplyTo(AgentAID* aid) {(*reply_to).insert(aid); }

void ACLMessage::deleteReplyTo(AgentAID* aid) {(*reply_to).erase(aid); }

void ACLMessage::clearReplyTo() {(*reply_to).clear(); }

std::set<AgentAID*>* ACLMessage::getAllReplyTo() {return reply_to; }

void ACLMessage::setReplyBy(long by) {reply_by = by; }

long ACLMessage::getReplyBy() {return reply_by; }

void ACLMessage::setInReplyTo(std::string str) {in_reply_to = str; }

std::string ACLMessage::getInReplyTo() {return in_reply_to; }

void ACLMessage::setReplyWith(std::string str) {reply_with = str; }

std::string ACLMessage::getReplyWith() {return reply_with; }

void ACLMessage::setConversationID(std::string str) {conversation_id = str; }

std::string ACLMessage::getConversationID() {return conversation_id; }

void ACLMessage::setProtocol(std::string str) {protocol = str; }

std::string ACLMessage::getProtocol() {return protocol; }

void ACLMessage::setOntology(std::string str) {ontology = str; }

std::string ACLMessage::getOntology() {return ontology; }

void ACLMessage::setEncoding(std::string str) {encoding = str; }

std::string ACLMessage::getEncoding() {return encoding; }

void ACLMessage::setLanguage(std::string str) {language = str; }

std::string ACLMessage::getLanguage() {return language; }

void ACLMessage::setContent(std::string cont) {content = cont; }

std::string ACLMessage::getContent() {return content; }

void ACLMessage::setSender(AgentAID* sender1) {sender = sender1; }


AgentAID* ACLMessage::getSender() {return sender; } 

void ACLMessage::addUserdefParam(UserdefParam* p) {(*params).insert(p);}

std::set<UserdefParam*>* ACLMessage:: getUserdefParams() {return params;}

void ACLMessage::setUserdefParams(std::set<UserdefParam*>* p) {params = p;}


std::string ACLMessage::getReplyBy1(){return reply_by1;}

void ACLMessage::setReplyBy1(std::string date1){reply_by1 = date1;}

}//end of acl namespace

}// end of fipa namespace
