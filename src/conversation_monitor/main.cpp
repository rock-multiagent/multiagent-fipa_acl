#include <iostream>
#include <fipa_acl/conversation_monitor.h>
#include <getopt.h>

using namespace fipa::acl;

int main(int argc, char** argv)
{
    std::string protocolfile = "request";
    bool print = false;
    bool directoryProvided = false;
    bool rimresTest = false;
    std::string directory;

    int opt;
    while( (opt = getopt(argc, argv, "pf:d:hr")) != -1)
    {
        switch(opt)
        {
            case 'r':
                rimresTest = true;
                break;
            case 'p':
                print = true;
                break;
            case 'f': 
                protocolfile = std::string(optarg);
                break;
            case 'd':
                directoryProvided = true;
                directory = std::string(optarg);
                break;

            case 'h': 
            default:
                printf("usage: %s [--print] [--directory] <protocolfile>\n", argv[0]);
                return 0;
                
        }
    }

    StateMachineBuilder builder;

    if(directoryProvided)
    {
        printf("Loading from directory %s\n", directory.c_str());
        StateMachineBuilder::setProtocolResourceDir(directory);
        builder.prepareProtocolsFromResourceDir();


        if(print)
        {
            printf("Get Statemachine for %s\n", protocolfile.c_str());
            StateMachine machine = builder.getStateMachine(protocolfile);
            //AgentID initiator("my-sender");
            //machine.setOwner(initiator);

            machine.print();
        }

        StateMachine machine;

        if(rimresTest)
        {
            machine = builder.getStateMachine("RIMRES");
            AgentID astrium("astrium");
            AgentID system("crex_0");

            machine.setOwner(system);

            machine.validate(true);

            ACLMessage msg0, msg1;
            msg0.setSender(astrium);
            msg0.addReceiver(system);
            msg0.setPerformative("request");

            msg1.setSender(system);
            msg1.addReceiver(astrium);
            msg1.setPerformative("inform");

            machine.consumeMessage(msg0);
            machine.consumeMessage(msg1);

            if(machine.isConversationOver())
            {
                printf("Conversation is over");
            } else {
                printf("Conversation is not over");
            }

            exit(0);
        }
    } else {

        printf("Load specification of: %s\n", protocolfile.c_str());
        try {
            StateMachine request = builder.loadSpecification(protocolfile);
            if(print) 
                request.print();
        } catch(std::runtime_error& e)
        {
            printf("Runtime error: %s\n", e.what());
        }
        
        printf("Retrieve functional state machine an perform a message consume\n");
        try {
            StateMachine request = builder.getFunctionalStateMachine(protocolfile);
            AgentID initiator("my-sender");
            request.setOwner(initiator);

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


        exit(0);
    }

    return 0;
}




