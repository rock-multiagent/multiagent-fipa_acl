/**
 * \author Thomas Roehr@dfki.de
 * \file main.cpp
 * \brief Message parser for FIPA ACL Messages, based on boost::spirit
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "grammar_bitefficient.h"

#include "message_printer.h"

////////////////////////////////////////////////////////////////////////////
//  Main program
////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
	char const* filename;
	if(argc > 1)
	{
		filename = argv[1];
	} else
	{
		std::cerr << "MessageParser core" << std::endl;
		std::cerr << "Usage: " << argv[0] << " binary-msg-file" << std::endl;
		return 1;
	}

	std::ifstream in(filename, std::ios_base::in);

	if (!in)
	{
		std::cerr << "Error: could not open input file: "
			<< filename << std::endl;
		return 1;
	}

	std::string storage; 
	
	// due to special characters don't use std::copy here
	char buffer;
	while(true)
	{
	   in.get(buffer);
	   if(!in.eof())
		   storage += buffer;
	   else
		   break;
	}

	typedef fipa::acl::bitefficient_grammar<std::string::const_iterator> bitefficient_grammar;
	bitefficient_grammar grammar;
	fipa::acl::Message parseTree;

	std::string::const_iterator iter = storage.begin();
	std::string::const_iterator end = storage.end();
	bool r = parse(iter, end, grammar, parseTree);

	if(r && iter == end)
	{
		std::cout << "------------------------------------\n";
		std::cout << " Parsing succeeded\n";
		std::cout << "------------------------------------\n";

		fipa::acl::MessagePrinter printer;
		printer.print(parseTree);

	        return 0;	
	} else {

		std::cout << " Parsing failed\n";
	}

    return 0;
}

