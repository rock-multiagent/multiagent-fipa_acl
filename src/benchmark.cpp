#include <fipa_acl/bitefficient_message.h>
#include <string.h>


int main()
{
    using namespace fipa::acl;
    ACLMessageOutputParser outputParser;
    ACLMessage msg("inform");
    uint32_t BUFFER_MAX = 8*1024*1024;
    char buffer[BUFFER_MAX+1];
    memset(&buffer, 1, BUFFER_MAX);
    buffer[BUFFER_MAX] = '\0';
    msg.setContent(std::string(buffer));

    outputParser.setMessage(msg);
    uint32_t epochs = 100;

    MessageParser inputParser;

    time_t start;
    time_t stop;
    std::string encodedMsg;

    {
        time(&start);
        for(int i = 0; i < epochs; i++)
            encodedMsg = outputParser.getBitMessage();
        time(&stop);

        double totaltime = difftime(stop, start);
        double timePerMsgInMs = (1000.0*totaltime)/(1.0*epochs);

        printf("Encoding time total: %.5f per msg: %.5f ms\n", totaltime, timePerMsgInMs);
    }


    {
        ACLMessage msg;
        printf("Encoded message size: %d\n", encodedMsg.size());
        /*
        for(size_t i = 0; i < encodedMsg.size(); i++)
        {
            printf("%02x ", encodedMsg.data()[i]);
        }
        printf("\n");
        */
        time(&start);
        for(int i = 0; i < epochs; i++)
        {
           if(!inputParser.parseData(encodedMsg, msg))
             printf("ERROR\n");
        }
        time(&stop);

        double totaltime = difftime(stop,start);
        double timePerMsgInMs = (1000.0*totaltime)/(1.0*epochs);

        printf("Decoding time total: %.5f per msg: %.5f ms\n", totaltime, timePerMsgInMs);
    }
}
