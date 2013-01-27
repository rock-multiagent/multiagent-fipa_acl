#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include "acl_message.h"
#include <boost/assign/list_of.hpp>
#include <boost/date_time.hpp>
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

ACLMessage::Performative ACLMessage::performativeFromString(const std::string& performative)
{
    std::map<ACLMessage::Performative, std::string>::const_iterator it = PerformativeTxt.begin();
    for(; it != PerformativeTxt.end(); ++it)
    {
        if(it->second == performative)
        {
            return it->first;
        }
    }
    std::string msg = "String '" + performative + "' does not match any existing performative definition";
    throw std::runtime_error(msg);
}

ACLMessage::ACLMessage()
{
    mPerformative = PerformativeTxt[INFORM];
}

ACLMessage::ACLMessage(Performative performative)
{
    mPerformative = PerformativeTxt[performative];
}

ACLMessage::ACLMessage(const std::string& perf) 
{
    if ( (perf.find_first_of(illegalWordChars) != std::string::npos) || (illegalWordStart.find_first_of(perf.c_str()[0]) != std::string::npos) )
    {
        char buffer[512];
        snprintf(buffer, 512,"Illegal characters in given performative: %s", perf.c_str());
	throw std::runtime_error(buffer);
    } else {
        mPerformative = perf;
    }
}

//ACLMessage& ACLMessage::operator=(const ACLMessage& mes)
//{
//    // checking against message1 = message1 case
//    if (this == &mes)
//       return *this;
//
//    //freeing any previously filled in values for the userdefined parameters, reply_to, receivers and sender fields
//    if (!mParameters.empty())
//    {
//       mParameters.clear();
//    }
//   
//    if (!mReplyTo.empty())
//    {
//        mReplyTo.clear();
//    }
//    
//    if (!mReceivers.empty())
//    {
//       mReceivers.clear();
//    }
//       
//     //building the copied message
//    mPerformative = mes.mPerformative;
//    mLanguage = mes.mLanguage;
//    mEncoding = mes.mEncoding;
//    mOntology = mes.mOntology;
//    mProtocol = mes.mProtocol;
//    mConversationId = mes.mConversationId;
//    mReplyWith = mes.mReplyWith;
//    mInReplyTo = mes.mInReplyTo;
//    mReplyBy1 = mes.mReplyBy1;
//    mContent = mes.mContent;
//    mSender = mes.mSender;
//         
//    mReceivers.insert(mReceivers.begin(), mes.mReceivers.begin(), mes.mReceivers.end());
//    mReplyTo.insert(mReplyTo.begin(), mes.mReplyTo.begin(), mes.mReplyTo.end());
//    mParameters.insert(mParameters.begin(), mes.mParameters.begin(), mes.mParameters.end());
//    
//    return *this;
//}


void ACLMessage::setPerformative(Performative perf)
{
    std::map<Performative, std::string>::iterator it;
    it = PerformativeTxt.find(perf);
    if(it == PerformativeTxt.end())
    {
	throw std::runtime_error("Cannot set performative. Performative unknown");
    }

    mPerformative = it->second;
}

void ACLMessage::setPerformative(const std::string& str) 
{
    if ( (str.find_first_of(illegalWordChars) != std::string::npos) || (illegalWordStart.find_first_of(str.c_str()[0]) != std::string::npos) )
    {

        char buffer[512];
        snprintf(buffer, 512,"Illegal characters in given performative: %s", str.c_str());
	throw std::runtime_error(buffer);
    }

    mPerformative = str; 
}

void ACLMessage::addReceiver(const AgentID& aid) 
{
    // prevent entering duplicates
    // NOTE: this function searches for and uses the overloaded == op not the resDepthEq()
    if ( find(mReceivers.begin(), mReceivers.end(),aid) == mReceivers.end() )
    {
        mReceivers.insert(mReceivers.begin(),aid); 
    }
}

void ACLMessage::deleteReceiver(const AgentID& aid) 
{
    // prevent entering duplicates
    // NOTE: this function searches for and uses the overloaded == op not the resDepthEq()
    std::vector<AgentID>::iterator it = find(mReceivers.begin(), mReceivers.end(),aid);
    if ( it != mReceivers.end())
    {
        mReceivers.erase(it);
    }
}

void ACLMessage::clearReceivers() 
{
    mReceivers.clear();
}

void ACLMessage::addReplyTo(const AgentID& aid) 
{
    // prevent entering duplicates
    // NOTE: this function searches for and uses the overloaded == op not the resDepthEq()
    if ( find(mReplyTo.begin(), mReplyTo.end(),aid) == mReplyTo.end() )
    {
        mReplyTo.insert(mReplyTo.begin(),aid); 
    }
}

void ACLMessage::deleteReplyTo(const AgentID& aid) 
{
    // prevent entering duplicates
    // NOTE: this function searches for and uses the overloaded == op not the resDepthEq()
    std::vector<AgentID>::iterator it = find(mReplyTo.begin(), mReplyTo.end(),aid);
    if (it != mReplyTo.end() )
    {
        mReplyTo.erase(it);
    }
}

void ACLMessage::clearReplyTo()
{
    mReplyTo.clear(); 
}

void ACLMessage::setProtocol(const std::string& str) 
{
    if ( (str.find_first_of(illegalWordChars) != std::string::npos) || (illegalWordStart.find_first_of(str.c_str()[0]) != std::string::npos) )
    {
        char buffer[512];
        snprintf(buffer, 512, "Protocol name contains illegal characters: %s", str.c_str());
        throw std::runtime_error(buffer);
    }

    mProtocol = str; 
}

bool ACLMessage::hasBinaryContent() const 
{
    // Check on the existance of 0x00 in the content
    return ( strlen(mContent.c_str()) != mContent.size() );
}

void ACLMessage::addUserdefParam(const UserdefParam& p) 
{
    if (find (mParameters.begin(),mParameters.end(),p) == mParameters.end() )
    {
        mParameters.insert(mParameters.begin(),p);
    }
}

void ACLMessage::setUserdefParams(const std::vector<UserdefParam>& p) 
{
    mParameters.clear();
    mParameters.insert(mParameters.begin(),p.begin(),p.end());
}

bool ACLMessage::operator==(const ACLMessage& other) const
{
    if (getPerformative().compare(other.getPerformative()))
        return false;
    if (getContent().compare(other.getContent()))
        return false;
    if (getLanguage().compare(other.getLanguage()))
        return false;
    if (getEncoding().compare(other.getEncoding()))
        return false;
    if (getOntology().compare(other.getOntology()))
        return false;
    if (getProtocol().compare(other.getProtocol()))
        return false;
    if (getConversationID().compare(other.getConversationID()))
        return false;
    if (getReplyWith().compare(other.getReplyWith()))
        return false;
    if (getInReplyTo().compare(other.getInReplyTo()))
        return false;
    if (getReplyBy() != other.getReplyBy())
        return false;
    if (!(getSender() == other.getSender()) )
        return false;
    
    // checking if receivers sets of the message are the same
    AgentIDList agentsA = getAllReceivers();
    AgentIDList agentsB = other.getAllReceivers();
    AgentIDList::iterator ait = agentsA.begin();
    AgentIDList::iterator bit = agentsB.begin();
    
    int found_one = 0; // flag variable to control flow through inner loops

    while (ait != agentsA.end())
    {
        found_one = 0;
        bit = agentsB.begin();
        while (bit != agentsB.end())
        {
	  if ( AgentID::compareEqual((*ait),(*bit),AgentID::msResolverComparisonDepth)) 
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
    
    if (!agentsA.empty() || !agentsB.empty())
        return false;
    
    //checking if reply_to sets of the message are  the same
    agentsA = getAllReplyTo();
    agentsB = other.getAllReplyTo();
    ait = agentsA.begin();
    bit = agentsB.begin();
    while (ait != agentsA.end())
    {
        found_one = 0;
        bit = agentsB.begin();
        while (bit != agentsB.end())
        {
	  if ( AgentID::compareEqual((*ait),(*bit),AgentID::msResolverComparisonDepth)) 
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
    
    
    std::vector<UserdefParam> paramsA = getUserdefParams();
    std::vector<UserdefParam> paramsB = other.getUserdefParams();
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

}//end of acl namespace

}// end of fipa namespace
