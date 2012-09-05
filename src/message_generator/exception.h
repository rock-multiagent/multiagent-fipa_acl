#ifndef FIPA_ACL_MSG_GENERATOR_EXCEPTION_H
#define FIPA_ACL_MSG_GENERATOR_EXCEPTION_H

#include <stdexcept>
#include <string>

class MessageGeneratorException : public std::runtime_error
{

public:
	MessageGeneratorException(const std::string& msg) : runtime_error(msg)
	{
	}

};

#endif // _MSG_GENERATOR_EXCEPTION_H_


