#include <iostream>
#include <fipa_acl/conversation_monitor.h>

using namespace fipa::acl;

int main(int argc, char** argv)
{
    std::string protocolfile;
    bool print = false;
    if(argc == 2)
    {
        protocolfile = std::string(argv[1]);
    } else if(argc == 3) {
        if(argv[1] == "--print") print = true;

        protocolfile = std::string(argv[2]);
    } else {
        printf("usage: %s [--print] <protocolfile>", argv[0]);
        return 0;
    }
    
    printf("Load specification of: %s\n", protocolfile.c_str());
    try {
        StateMachineBuilder builder;
        StateMachine* request = builder.loadSpecification(protocolfile);
        if(print) 
            request->print();
        if(request)
            delete request;
    } catch(std::runtime_error& e)
    {
        printf("Runtime error: %s\n", e.what());
    }
    
    printf("Retrieve functional state machine an perform a message consume\n");
    try {
        StateMachineBuilder builder;
        StateMachine* request = builder.getFunctionalStateMachine(protocolfile);
        if(request)
        {
            if( request->checkIfRoleExists("initiator"))
                printf("initiator exists\n");
            else
                printf("initiator does not exist\n");

            if(print)
                request->print();
            delete request;
        }
    } catch(std::runtime_error& e)
    {
        printf("Runtime error %s\n", e.what());
    }
    return 0;
}




