#include <iostream>
#include <fipa_acl/conversation_monitor.h>

using namespace fipa::acl;

int main(int argc, char** argv)
{
    std::string protocolfile;
    bool print = false;
    bool isDirectory = false;
    if(argc == 2)
    {
        protocolfile = std::string(argv[1]);
    } else if(argc == 3) {
        if(std::string(argv[1]) == "--print") print = true;
        if(std::string(argv[1]) == "--directory") isDirectory = true;

        protocolfile = std::string(argv[2]);
    } else {
        printf("usage: %s [--print] [--directory] <protocolfile>\n", argv[0]);
        return 0;
    }

    if(isDirectory)
    {
        printf("Loading from directory %s\n", protocolfile.c_str());
        StateMachineBuilder::setProtocolResourceDir(protocolfile);
        StateMachineBuilder builder;
        builder.prepareProtocolsFromResourceDir();

        StateMachine request = builder.getStateMachine("request");
        request.print();
        exit(0);
    }
    
    printf("Load specification of: %s\n", protocolfile.c_str());
    try {
        StateMachineBuilder builder;
        StateMachine request = builder.loadSpecification(protocolfile);
        if(print) 
            request.print();
    } catch(std::runtime_error& e)
    {
        printf("Runtime error: %s\n", e.what());
    }
    
    printf("Retrieve functional state machine an perform a message consume\n");
    try {
        StateMachineBuilder builder;
        StateMachine request = builder.getFunctionalStateMachine(protocolfile);
        if( request.checkIfRoleExists("initiator"))
            printf("initiator exists\n");
        else
            printf("initiator does not exist\n");

        if(print)
            request.print();
    } catch(std::runtime_error& e)
    {
        printf("Runtime error %s\n", e.what());
    }
    return 0;
}




