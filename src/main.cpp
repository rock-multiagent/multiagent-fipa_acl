/**
 * \author Thomas Roehr@dfki.de
 * \file main.cpp
 * \brief Message parser for FIPA ACL Messages, based on boost::spirit
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "grammar_bitefficient_test.h"
//#include "grammar_bitefficient.h"


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
		std::cerr << "Error: no input file provided." << std::endl;
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
//	in.unsetf(std::ios::skipws); // No white space skipping
	std::copy(
		std::istream_iterator<char>(in),
		std::istream_iterator<char>(),
		std::back_inserter(storage));

	typedef fipa::acl::bitefficient_grammar<std::string::const_iterator> bitefficient_grammar;
	bitefficient_grammar grammar;
	fipa::acl::Message parseTree;

	using boost::spirit::ascii::space;
	std::string::const_iterator iter = storage.begin();
	std::string::const_iterator end = storage.end();
	bool r = phrase_parse(iter, end, grammar, space, parseTree);

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

