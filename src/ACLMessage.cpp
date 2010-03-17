#include <iostream>
#include <set>
#include <string>
#include "ACLMessage.h"

const std::string ACLMessage::perfs[22] = {"accept-proposal","agree","cancel","cfp","confirm","disconfirm","failure","inform","inform-if","inform-ref","not-understood","propagate","propose","proxy","query-if","query-ref","refuse","reject-proposal","request","request-when","request-whenever","subscribe"};

  
void ACLMessage::initializeObject()
{
	AgentAID* sender = NULL;
        receivers = new std::set<AgentAID*>();
	if (!(*receivers).empty()) (*receivers).clear();
        reply_to = new std::set<AgentAID*>();
	if (!(*reply_to).empty()) (*reply_to).clear();
         language = std::string("\0");
         encoding = std::string("\0");
         ontology = std::string("\0");
         protocol = std::string("\0");
         conversation_id = std::string("\0");
         reply_with = std::string("\0");
         in_reply_to = std::string("\0");
         reply_by = -1;
         reply_by1 = std::string("\0");
        params = new std::set<UserdefParam*>();
		
	if (!(*params).empty()) (*params).clear();
		
        
         content = std::string("\0");
}
ACLMessage::ACLMessage() 
{/*
	AgentAID* sender = NULL;
        receivers = new std::set<AgentAID*>();
	if (!(*receivers).empty()) (*receivers).clear();
        reply_to = new std::set<AgentAID*>();
	if (!(*reply_to).empty()) (*reply_to).clear();
         language = std::string("\0");
         encoding = std::string("\0");
         ontology = std::string("\0");
         protocol = std::string("\0");
         conversation_id = std::string("\0");
         reply_with = std::string("\0");
         in_reply_to = std::string("\0");
         reply_by = -1;
         reply_by1 = std::string("\0");
        params = new std::set<UserdefParam*>();
		
	if (!(*params).empty()) (*params).clear();
		
        
         content = std::string("\0");
*/
initializeObject();
}

ACLMessage::ACLMessage(int perf)

{
initializeObject();
performative = ACLMessage::perfs[perf];
}

ACLMessage::ACLMessage(std::string perf) {performative = perf; }

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
