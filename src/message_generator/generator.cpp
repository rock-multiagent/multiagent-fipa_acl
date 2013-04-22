/**
* \file generator.cpp
* \author Thomas Roehr, thomas.roehr@dfki.de
* \brief Console program that asks for settings of the fipa message
* and output the corresponding bytestream of a bitefficient message
*
*
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <map>
#include <fipa_acl/message_generator/message_generator.h>

std::map<std::string, std::string> options;

bool usage(int argc, char** argv)
{
    int option;

    if(argc <= 1)
    {
            printf("usage: %s -p <performative> -c <content>\n", argv[0]);
            exit(0);
    }

    while( (option = getopt(argc, argv, "hp:c:")) != -1 )
    {
        switch(option)
        {

            case 'p':
                options["performative"] = optarg;
                break;
            case 'c':
                options["content"] = optarg;
                break;
                        case 'h':
                printf("usage: %s -p <performative> -c <content>\n", argv[0]);
                exit(0);
            default:
                break;
        }
    }

    return true;
}

int main(int argc, char** argv)
{
    if( !usage(argc, argv) )
    {
        return 0;
    }

    fipa::acl::ACLMessage msg(options["performative"]);
    msg.setContent(options["content"]);
     
    std::string bitefficientMsg = fipa::acl::MessageGenerator::create(msg, fipa::acl::representation::BITEFFICIENT);
    int size = bitefficientMsg.size();

    for(int i = 0; i < size; i++)
    {
     fprintf(stderr, "%x ", bitefficientMsg[i]);
    }

    fprintf(stderr, "\n");


    return 0;
}




