#include "message_printer.h"

namespace fipa { namespace acl {

MessagePrinter::MessagePrinter()
{
}

MessagePrinter::~MessagePrinter()
{
}

void MessagePrinter::print(const fipa::acl::Message& msg) const
{	
	printf("FIPA Message read:\n");
	printf("id:          %x\n", msg.header.id);

		if( (msg.header.id & ! 0xfa) == 0) printf("\tid is FA\n");
		else if( (msg.header.id & ! 0xfb) == 0 ) printf("\tid is FB\n");
		else if( (msg.header.id & ! 0xfc) == 0 ) printf("\tid is FC\n"); 
		else printf("\tWARNING: id is unknown\n");


	printf("version:                  %x\n", msg.header.version); 
	printf("performative(type):       %s\n", msg.type.c_str());
	
	printf("msg-parameters count:     %d\n",(int) msg.parameters.size());
	for(uint32_t i=0; i < msg.parameters.size(); i++)
	{
		message::Parameter mp = msg.parameters[i];
		printf("\tparameter #%d:  %s\n",i, mp.name.c_str()); 
	
		if(mp.name == "sender")
		{
			AgentIdentifierPrinter aidPrinter;
			fipa::acl::AgentIdentifier& aid = boost::get<fipa::acl::AgentIdentifier>(mp.data);
			aidPrinter.print(aid);
		} else if(mp.name == "receiver")
		{
			AgentIdentifierPrinter aidPrinter;
			std::vector<fipa::acl::AgentIdentifier> aids = boost::get<std::vector<fipa::acl::AgentIdentifier> >(mp.data);
			int length = aids.size();

			printf("Receiver: \n");
			for(int i = 0; i < length; i++)
			{
				aidPrinter.print(aids[i]);
			}


		} else if(mp.name == "content")
		{	
			fipa::acl::ByteSequence bs = boost::get<fipa::acl::ByteSequence>(mp.data);
			printf("encoding: %s\n", bs.encoding.c_str());

			std::string printBytes = bs.toPrettyString();
			printf("content: %s\n", printBytes.c_str());
		} else if(mp.name == "reply-with")
		{
			std::string replyWith = boost::get<std::string>(mp.data);
			printf("ReplyWith: %s\n", replyWith.c_str());
		} else if(mp.name == "reply-by")
		{
			fipa::acl::DateTime dt = boost::get<fipa::acl::DateTime>(mp.data);
			std::string dtString = dt.toString();
			printf("DateTime: %s\n", dtString.c_str());

		} else if(mp.name == "in-reply-to")
		{
			std::string inReplyTo = boost::get<std::string>(mp.data);	
			printf("InReplyTo: %s\n", inReplyTo.c_str());

		} else if(mp.name == "reply-to")
		{
			AgentIdentifierPrinter aidPrinter;
			std::vector<fipa::acl::AgentIdentifier> aids = boost::get<std::vector<fipa::acl::AgentIdentifier> >(mp.data);
			int length = aids.size();

			printf("ReplyTo: \n");
			for(int i = 0; i < length; i++)
			{
				aidPrinter.print(aids[i]);
			}

			
		} else if(mp.name == "language")
		{
			std::string language = boost::get<std::string>(mp.data);	
			printf("Language: %s\n", language.c_str());


		} else if(mp.name == "encoding")
		{	
			std::string encoding = boost::get<std::string>(mp.data);	
			printf("Encoding: %s\n", encoding.c_str());


		} else if(mp.name == "ontology")
		{

			std::string ontology = boost::get<std::string>(mp.data);	
			printf("Ontology: %s\n", ontology.c_str());


		} else if(mp.name == "protocol")
		{
			std::string protocol = boost::get<std::string>(mp.data);	
			printf("Protocol: %s\n", protocol.c_str());


		} else if(mp.name == "conversation-id")
		{
			std::string conversationId = boost::get<std::string>(mp.data);	
			printf("Conversation-id: %s\n", conversationId.c_str());


		}

	}

}

}} // namespace fipa::acl
