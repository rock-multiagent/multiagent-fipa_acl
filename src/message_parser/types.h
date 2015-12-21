#ifndef _FIPAACL_TYPES_H_
#define _FIPAACL_TYPES_H_

#include <stdio.h>
#include <vector>
#include <string>

#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>
#include <boost/bind.hpp>

#include <fipa_acl/message_parser/date_time.h>
#include <fipa_acl/message_parser/byte_sequence.h>
#include <fipa_acl/message_parser/agent_id.h>
#include <fipa_acl/message_parser/parameter.h>

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
struct Header
{
	char id;
	char version;

        std::string toString() const
        {
                char tmp[100];
                snprintf(tmp,100, "Header<id=%x, version=%x>", id, version);
                return std::string(tmp);
        }

};

namespace message {

struct Parameter
{
	std::string name;
	ParameterValue data;

        std::string toString() const
        {
                std::string tmp = name;
                tmp += "<" + boost::apply_visitor(ParameterPrinter(), data) + ">";
                return tmp;
        }

};

}

namespace envelope
{
    struct Parameter
    {
        std::string name;
        ParameterValue data;

        std::string toString() const
        {
            return name + "<" + data + ">";
        }
    };

}

// Define the final message structure here
struct Message
{
	Message() : type(""), parameters() {}

	Header header;
	std::string type;
	std::vector<message::Parameter> parameters;
};



} // end namespace acl
} // end namespace fipa

#endif // _FIPAACL_TYPES_H_
