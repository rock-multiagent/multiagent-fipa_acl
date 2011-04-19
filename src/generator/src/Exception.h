#ifndef _MSG_GENERATOR_EXCEPTION_H_
#define _MSG_GENERATOR_EXCEPTION_H_

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


