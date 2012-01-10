#include <fipa_acl/bitefficient_message.h>
#include <string.h>
#include <algorithm>
#include <math.h>
#include <sys/time.h>

/* Subtract the `struct timeval' values X and Y,
   storing the result in RESULT.
   Return 1 if the difference is negative, otherwise 0.  */
// from http://www.cs.utah.edu/dept/old/texinfo/glibc-manual-0.02/library_19.html#SEC313
int timeval_subtract (struct timeval* result, struct timeval* x,struct timeval* y)
{
    /* Perform the carry for the later subtraction by updating y. */
    if (x->tv_usec < y->tv_usec)
    {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }

    if (x->tv_usec - y->tv_usec > 1000000)
    {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }
  
    /* Compute the time remaining to wait.
       tv_usec is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;
  
    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
}


int main(int argc, char** argv)
{
    if(argc != 3)
    {
        printf("usage: %s <content-size-in-byte> <epochs>\n", argv[0]);
        printf("output will be: <content-size in byte> <encoded-msg-size in> <header-percent> <encoding-time in ms/msg> <decoding-time in ms/msg> <epochs>\n");
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
    msg.setPerformative(fipa::acl::ACLMessage::REQUEST);
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
    msg.setContent(buffer);

    outputParser.setMessage(msg);
    // 1 MB ~ 10 ms > 200 runs
    // 0 MB ~ 0 ms > 200000
    int32_t epochs = atoi(argv[2]);
    
    MessageParser inputParser;

    time_t start;
    time_t stop;
    std::string encodedMsg;

    double encodingMsPerMsg;
    double encodedMsgSize;
    double decodingMsPerMsg;

    {
        struct timeval start, stop, diff;
        gettimeofday(&start, 0);
        for(int i = 0; i < epochs; i++)
           encodedMsg = outputParser.getBitMessage();
        gettimeofday(&stop, 0);
	
        timeval_subtract(&diff, &stop, &start);

        double totaltime = diff.tv_sec*1000 + diff.tv_usec/1000.0;
        //double totaltime = difftime(stop, start);
        double timePerMsgInMs = totaltime/(1.0*epochs);

        //printf("Encoding time total: %.5f per msg: %.5f ms\n", totaltime, timePerMsgInMs);
        encodingMsPerMsg = timePerMsgInMs;
    }

    {
        ACLMessage msg;
        encodedMsgSize = encodedMsg.size();
        struct timeval start, stop, diff;
        gettimeofday(&start, 0);
        for(int i = 0; i < epochs; i++)
        {
           if(!inputParser.parseData(encodedMsg, msg))
             printf("ERROR\n");
        }
        gettimeofday(&stop, 0);
        timeval_subtract(&diff, &stop, &start);

        double totaltime = diff.tv_sec*1000 + diff.tv_usec/1000.0;
        double timePerMsgInMs = totaltime/(1.0*epochs);

        //printf("Decoding time total: %.5f per msg: %.5f ms\n", totaltime, timePerMsgInMs);
        decodingMsPerMsg = timePerMsgInMs;
    }

    double overheadInPercent = (encodedMsgSize - BUFFER_MAX)/(1.0*encodedMsgSize);

    printf("%d %d %.6f %.6f %.6f %d\n", BUFFER_MAX, (int) encodedMsg.size(), overheadInPercent, encodingMsPerMsg, decodingMsPerMsg, epochs);

    return 0;
}
