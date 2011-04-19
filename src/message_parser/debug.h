#ifndef __FIPA_ACL_MESSAGEPARSER_DEBUG_H_
#define __FIPA_ACL_MESSAGEPARSER_DEBUG_H_

#include <iosfwd>
#include <utility>

#include <fipa_acl/message_parser/types.h>

template< typename C, typename E>
std::basic_ostream<C, E>& operator<<(std::basic_ostream<C,E>& out, fipa::acl::Header h)
{
	        return out << h.toString();
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
                return out << "Parameter" << tmp;
}

template< typename C, typename E>
std::basic_ostream<C, E>& operator<<(std::basic_ostream<C,E>& out, const fipa::acl::MessageParameter& p)
{
	        return out << "MessageParameter<" << p.toString() << ">";

}


#endif 
