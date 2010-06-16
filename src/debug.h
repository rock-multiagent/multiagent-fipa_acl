#ifndef __FIPA_ACL_MESSAGEPARSER_DEBUG_H_
#define __FIPA_ACL_MESSAGEPARSER_DEBUG_H_

#include <iosfwd>
#include <utility>

#include <types.h>

template< typename C, typename E>
std::basic_ostream<C, E>& operator<<(std::basic_ostream<C,E>& out, Header h)
{
	        return out << h.toString();
}

template< typename C, typename E>
std::basic_ostream<C, E>& operator<<(std::basic_ostream<C,E>& out, MessageParameter p)
{
	        return out << "MessageParameter<" << p.toString() << ">";

}

template<typename C, typename E>
std::basic_ostream<C, E>& operator<<(std::basic_ostream<C,E>& out, DateTime dt) 
{
	        std::string dtString = dt.toString();
		        return out << "DateTime<" << dtString << ">";
}

template<typename C, typename E>
std::basic_ostream<C, E>& operator<<(std::basic_basic_ostream<C,E>& out, Time t)
{
	        return out << "Time<" << ">";
}



#endif 
