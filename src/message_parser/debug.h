#ifndef __FIPA_ACL_MESSAGEPARSER_DEBUG_H_
#define __FIPA_ACL_MESSAGEPARSER_DEBUG_H_

#include <iosfwd>
#include <utility>

#include <fipa_acl/message_parser/types.h>
#warning "debug.h ENABLED"

/**
 * Alternatively to putting the stream operators in std namespace 
 * do as recommended by Helmut Kaiser: 
 * http://stackoverflow.com/questions/5286720/how-to-define-streaming-operator-for-boostspiritqi-and-stdlist-container
 *
 *  // your specialization needs to be in namespace boost::spirit::traits
 *  namespace boost { namespace spirit { namespace traits
 *  {
 *      template <typename Out>
 *      struct print_attribute_debug<Out, TlstValues>
 *      {
 *          static void call(Out& out, TlstValues const& val)
 *          {
 *              // do your output here; Out is a std::ostream
 *          }
 *      };
 *  }}}
 */
namespace std
{

template< typename C, typename E>
std::basic_ostream<C, E>& operator<<(std::basic_ostream<C,E>& out, fipa::acl::Header h)
{
	        return out << "Header<" << h.toString() << ">";
}

template<typename C, typename E>
std::basic_ostream<C, E>& operator<<(std::basic_ostream<C,E>& out, fipa::acl::DateTime dt) 
{
	        std::string dtString = dt.toString();
		return out << "DateTime<" << dtString << ">";
}

template<typename C, typename E>
std::basic_ostream<C, E>& operator<<(std::basic_ostream<C,E>& out, fipa::acl::Time t)
{
	        return out << "Time<" << ">";
}

template< typename C, typename E>
std::basic_ostream<C, E>& operator<<(std::basic_ostream<C,E>& out, const fipa::acl::ParameterValue& p)
{
	        std::string tmp = boost::apply_visitor(fipa::acl::ParameterPrinter(), p);
                return out << "ParameterValue<" << tmp << ">";
}

template< typename C, typename E>
std::basic_ostream<C, E>& operator<<(std::basic_ostream<C,E>& out, fipa::acl::MessageParameter p)
{
	        return out << "MessageParameter<" << p.toString() << ">";
}

template< typename C, typename E>
std::basic_ostream<C, E>& operator<<(std::basic_ostream<C,E>& out, fipa::acl::AgentIdentifier p)
{
                return out << "AgentId<" << p.name << ">";
}

template< typename C, typename E>
std::basic_ostream<C, E>& operator<<(std::basic_ostream<C,E>& out, const fipa::acl::UserDefinedParameter& p)
{
                return out << "UserDefinedParameter<" << p; 
}

template< typename C, typename E>
std::basic_ostream<C, E>& operator<<(std::basic_ostream<C,E>& out, const fipa::acl::ByteSequence& b)
{
                return out << "Bytesequence<" << b.toString() << ">";
}

template< typename C, typename E>
std::basic_ostream<C, E>& operator<<(std::basic_ostream<C,E>& out, const fipa::acl::DateTime& d)
{
                return out << "DateTime<" << d.toString() << ">"; 
}

template< typename C, typename E>
std::basic_ostream<C, E>& operator<<(std::basic_ostream<C,E>& out, const fipa::acl::Resolver& r)
{
                fipa::acl::AgentIdentifier agentId = r.get();
                return out << "Resolver<" << agentId.name << ">"; 
}

}

#endif 
