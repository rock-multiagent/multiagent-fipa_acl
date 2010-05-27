#ifndef _RIMRES_FIPAACL_TYPES_H_
#define _RIMRES_FIPAACL_TYPES_H_

#include <stdio.h>
#include <vector>
#include <string>
#include <ctime>


#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>
#include <boost/bind.hpp>
#include <boost/cstdint.hpp>

namespace fipa
{

//#############################################################
// The FIPA Bit Message Representation in Bit-Efficient Encoding
//#############################################################

namespace acl
{

// Embedd only types into the BOOST_FUSION_ADAPT_STRUCT
// So perform all definitions here
//
typedef struct
{
	char id;
	char version;
} Header;

typedef boost::variant<boost::uint_least8_t,boost::uint_least16_t, boost::uint_least32_t> LengthValue;


typedef boost::variant<std::string, std::vector<unsigned char> > ByteString;

/**
* \class ByteStringPrinter
* \brief Printer using the visitor pattern for the ByteString type
*/
class ByteStringPrinter : public boost::static_visitor<std::string>
{

	std::string encoding;
public: 

	ByteStringPrinter(std::string enc) : encoding(enc) {}
	
	/**
	* Build a string for the given type
	* \param s One of the types for the variant that this printer is implemented for
	*/
	std::string operator()(std::string s) const
	{
		return s;
	}
	
	/**
	* Build a string from a raw byte sequence, i.e. the following output format will be used, 
        * embedding encoding information
        *  HEX(dword)[0a af 02 10 ... 01]
	* \param vector One of the types for the variant that this printer is implemented for
	*/
	std::string operator()(std::vector<unsigned char> vector) const
	{
		int length = vector.size();
		std::string tmp("HEX(");
		tmp += encoding;
		tmp +=")[";

		for(int i = 0; i < length; i++)
		{
			char currentChar; 
			sprintf(&currentChar, "%x", vector[i]);
			tmp += tmp;
			if( i%2 == 0 )
				tmp += " ";
		}

		tmp += "]";

		return std::string(tmp);
	}
};

// Bytesequence - whereas the encoding is defined by a natural number, i.e. 
// postprocessing has to be performed 
struct ByteSequence
{
	std::string encoding;
	fipa::acl::LengthValue length;
	fipa::acl::ByteString bytes;

	std::string toString()
	{
		std::string tmp;
		// Since bytes is a variant we apply the visitor pattern here
		tmp += boost::apply_visitor( ByteStringPrinter(encoding), bytes);
		return tmp;
	}

};

struct AgentID;
//struct MessageParameter;

typedef boost::recursive_wrapper<AgentID> Resolver;

// To avoid dealing with circular dependecies we use a 'similar' definition to ParameterValue 
typedef boost::variant<std::string, fipa::acl::Resolver, std::vector<fipa::acl::Resolver>, fipa::acl::ByteSequence, fipa::acl::ByteString > UserDefinedParameterValue;

typedef struct
{
	std::string name;
	fipa::acl::UserDefinedParameterValue data;
} UserDefinedParameter;


struct AgentID
{
	std::string name;
	std::vector<std::string> addresses;	
	std::vector<fipa::acl::Resolver> resolvers;
	std::vector<fipa::acl::UserDefinedParameter> parameters;
};

struct Time : public std::tm
{
	// inheriting from struct tm

	// Extending milliseconds
	int tm_msec;
}; 

struct DateTime
{
   char relative;
   Time dateTime; 
   char timezone;

   DateTime() : relative(0), timezone(0) {}

   std::string toString()
   {
	char buffer[512];
	// %Z missing so far
	strftime(buffer,512, "%Y-%m-%dT%H:%M:%S", &dateTime);

	std::string formattedOutput;
	formattedOutput += relative;
	formattedOutput += std::string(buffer);
	formattedOutput += timezone;

	return formattedOutput;
   }

}; 

typedef boost::variant<std::string, fipa::acl::AgentID, std::vector<fipa::acl::AgentID>, fipa::acl::ByteSequence, fipa::acl::DateTime, fipa::acl::ByteString > ParameterValue;

typedef struct
{
	std::string name;
	fipa::acl::ParameterValue data;
} MessageParameter;


// Define the final message structure here
struct Message
{
	Message() : type(""), parameters() {}

	Header header;
	std::string type;
	std::vector<fipa::acl::MessageParameter> parameters;
};

class AgentIDPrinter
{
	public:
	AgentIDPrinter() {}
	
	void print(const fipa::acl::AgentID& aid)
	{
				printf("\t\tname: %s\n", aid.name.c_str());
				for(int a=0; a < aid.addresses.size(); a++)
				{	
					printf("\t\taddresses: %s\n", aid.addresses[a].c_str());
				}
				printf("\t\tresolvers:\n");
				for(int r=0; r < aid.resolvers.size(); r++)
				{
					fipa::acl::AgentID resolver = aid.resolvers[r].get();
					print(resolver);
				}

	}

};

class MessagePrinter 
{
	public:
	MessagePrinter()
	{
	}

	void print(const fipa::acl::Message& msg)
	{	
		printf("FIPA Message read:\n");
		printf("id:          %x\n", msg.header.id);

			if( (msg.header.id & ! 0xfa) == 0) printf("\tid is FA\n");
			else if( (msg.header.id & ! 0xfb) == 0 ) printf("\tid is FB\n");
			else if( (msg.header.id & ! 0xfc) == 0 ) printf("\tid is FC\n"); 
			else printf("\tWARNING: id is unknown\n");
	

		printf("version:                  %x\n", msg.header.version); 
		printf("performative(type):       %s\n", msg.type.c_str());
		
		printf("msg-parameters count:     %d\n", msg.parameters.size());
		for(int i=0; i < msg.parameters.size(); i++)
		{
			MessageParameter mp = msg.parameters[i];
			printf("\tparameter #%d:  %s\n",i, mp.name.c_str()); 
		
			if(mp.name == "sender")
			{
				AgentIDPrinter aidPrinter;
				fipa::acl::AgentID& aid = boost::get<fipa::acl::AgentID>(mp.data);
				aidPrinter.print(aid);
			} else if(mp.name == "receiver")
			{
				AgentIDPrinter aidPrinter;
				std::vector<fipa::acl::AgentID> aids = boost::get<std::vector<fipa::acl::AgentID> >(mp.data);
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

				std::string printBytes = bs.toString();
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
				AgentIDPrinter aidPrinter;
				std::vector<fipa::acl::AgentID> aids = boost::get<std::vector<fipa::acl::AgentID> >(mp.data);
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
};

} // end namespace acl
} // end namespace fipa

#endif // _RIMRES_FIPAACL_TYPES_H_
