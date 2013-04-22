/**
 * \author Thomas Roehr, thomas.roehr@dfki.de
 * \file message_parser/main.cpp
 * \brief Message parser for FIPA ACL Messages, based on boost::spirit
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include <fipa_acl/message_generator/message_generator.h>

#include <fipa_acl/message_parser/bitefficient/grammar_bitefficient.h>
#include <fipa_acl/message_parser/message_printer.h>

std::map<std::string, std::string> options;


void info(char** argv)
{
	printf("usage: %s -b <binary-message-file> -i \n", argv[0]);
	printf("-b require a binary message file as input\n");
	printf("-i interactive mode\n");
	exit(0);
}

bool usage(int argc, char** argv)
{
	int option;

	while( (option = getopt(argc, argv, "hb:i")) != -1 )
	{
		switch(option)
		{
			case 'b':
				options["binary-msg-file"] = optarg;
				break;
			case 'i':
				options["interactive"] = "Y";
				break;
                        case 'h':
				info(argv);
				break;
			default:
				break;
		}
	}

	return true;
}

bool parseMsg(const std::string& storage)
{
	typedef fipa::acl::bitefficient::Message<std::string::const_iterator> bitefficient_message_grammar;

	bitefficient_message_grammar grammar;
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

	        return true;
	} else {

		std::cout << " Parsing failed\n";
		return false;
	}
}


bool parseFile(const std::string& filename)
{
	std::ifstream in(filename.c_str(), std::ios_base::in);

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

	
	return parseMsg(storage);
}


bool parseInteractive()
{

	fipa::acl::ACLMessage msg; 
	char buffer[512];
	printf("Performative: \n");
	//scanf(buffer, "%s");
	msg.setPerformative("inform");
	
	printf("Content: \n");
	getchar();
	//scanf(buffer, "%s");
	msg.setContent("");
	
	fipa::acl::MessageGenerator generator;
	std::string bitefficientMessage = generator.create(msg, fipa::acl::representation::BITEFFICIENT);
	
	return parseMsg(bitefficientMessage);
}




////////////////////////////////////////////////////////////////////////////
//  Main program
////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{

	if( !usage(argc, argv) )
		return 0;

	std::string filename;
	if( ( filename = options["binary-msg-file"]) != "")
	{ 
		parseFile(filename);
	} else if (options["interactive"] != "")
	{
		parseInteractive();	
	} else
	{
		info(argv);
	}

    return 0;
}

