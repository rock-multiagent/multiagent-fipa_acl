#include <fipa_acl/bitefficient_message.h>
#include <string.h>
#include <algorithm>
#include <math.h>


int main(int argc, char** argv)
{

    if(argc == 0)
    {
        printf("usage: %s <content-size-in-byte>\n", argv[1]);
        printf("output will be: <content-size> <encoded-msg-size> <header-percent> <encoding-time> <decoding-time> <epochs>\n");
        exit(0);
    }

    using namespace fipa::acl;
    ACLMessageOutputParser outputParser;
    ACLMessage msg("inform");
    AgentID origin("proxy");
    AgentID receiver("crex_0");

    msg.setSender(origin);
    msg.addReceiver(receiver);
    msg.addReplyTo(origin);
    msg.setPerformative(fipa::acl::ACLMessage::perfs[fipa::acl::ACLMessage::REQUEST]);
    msg.setProtocol(std::string("RIMRES"));
    msg.setLanguage(std::string("test language"));
    msg.setEncoding(std::string("test encoding"));
    msg.setOntology(std::string("test ontology"));
    msg.setReplyWith(std::string("test reply_with"));
    msg.setReplyBy1(std::string("2010-12-23T12:00:37:980"));
    msg.setConversationID(std::string("test conversationID"));

    uint32_t BUFFER_MAX = atoi(argv[1]);
    char buffer[BUFFER_MAX+1];
    memset(buffer, 1, BUFFER_MAX);
    buffer[BUFFER_MAX] = '\0';
    msg.setContent(std::string(buffer));

    outputParser.setMessage(msg);
    // 1 MB ~ 10 ms > 200 runs
    // 0 MB ~ 0 ms > 200000
    int32_t epochs = 1024*100;
    if(BUFFER_MAX <= 1024*1024)
    {
            epochs = 1024*100.0;
    } else {
        epochs = 100;
    }
    epochs = std::max(epochs,100);

    MessageParser inputParser;

    time_t start;
    time_t stop;
    std::string encodedMsg;

    double encodingMsPerMsg;
    double encodedMsgSize;
    double decodingMsPerMsg;

    {
        time(&start);
        for(int i = 0; i < epochs; i++)
            encodedMsg = outputParser.getBitMessage();
        time(&stop);

        double totaltime = difftime(stop, start);
        double timePerMsgInMs = (1000.0*totaltime)/(1.0*epochs);

        //printf("Encoding time total: %.5f per msg: %.5f ms\n", totaltime, timePerMsgInMs);
        encodingMsPerMsg = timePerMsgInMs;
    }


    {
        ACLMessage msg;
        //printf("Encoded message size: %d\n", encodedMsg.size());
        encodedMsgSize = encodedMsg.size();
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

        //printf("Decoding time total: %.5f per msg: %.5f ms\n", totaltime, timePerMsgInMs);
        decodingMsPerMsg = timePerMsgInMs;
    }

    double overheadInPercent = (encodedMsg.size() - BUFFER_MAX)/(1.0*encodedMsg.size());

    printf("%d %d %.6f %.6f %.6f %d\n", BUFFER_MAX, (int) encodedMsg.size(), overheadInPercent, encodingMsPerMsg, decodingMsPerMsg, epochs);

    return 0;
}
