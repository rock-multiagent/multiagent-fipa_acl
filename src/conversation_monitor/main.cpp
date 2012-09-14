#include <iostream>
#include <fipa_acl/conversation_monitor.h>
#include <getopt.h>

using namespace fipa::acl;

int main(int argc, char** argv)
{
    std::string protocolfile;

    int opt;
    while( (opt = getopt(argc, argv, "pf:d:hr")) != -1)
    {
        switch(opt)
        {
            case 'f': 
                protocolfile = std::string(optarg);
                break;
            case 'h': 
            default:
                printf("usage: %s [-f <protocolfile>]\n", argv[0]);
                return 0;
                
        }
    }

    printf("Load specification of: %s\n", protocolfile.c_str());
    try {
        StateMachineReader reader;
        StateMachine statemachine = reader.loadSpecification(protocolfile);
        printf("Statemachine: \n%s", statemachine.toString().c_str());
    } catch(std::runtime_error& e)
    {
        printf("Runtime error: %s\n", e.what());
    }
        
    return 0;
}




