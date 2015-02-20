#include <fipa_acl/fipa_acl.h>
#include <string.h>
#include <algorithm>
#include <math.h>
#include <sys/time.h>
#include <numeric/Stats.hpp>

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
    if(argc < 3)
    {
        printf("usage: %s <content-size-in-byte> <epochs>\n", argv[0]);
        printf("output will be: <encoding> <content-size in byte> <encoded-msg-size in bytes > <overhead-percent> <encoding-time in ms/msg> <decoding-time in ms/msg> <epochs>\n");
        exit(0);
    }

    // 1 MB ~ 10 ms > 200 runs
    // 0 MB ~ 0 ms > 200000
    uint32_t BUFFER_MAX = atoi(argv[1]);
    int32_t epochs = atoi(argv[2]);

    using namespace fipa::acl;
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
    base::Time time = base::Time::fromString("20101223-12:00:37:980", base::Time::Milliseconds);
    msg.setReplyBy(time);
    msg.setConversationID(std::string("test conversationID"));

    // leaving out performative but assuming 1 bytes for encoding (23
    // performatives)
    // leaving out time but assuming 8 bytes for encoding, s. below
    std::string info;
    info += origin.getName();
    info += receiver.getName();
    info += origin.getName();
    info += msg.getLanguage();
    info += msg.getEncoding();
    info += msg.getOntology();
    info += msg.getReplyWith();
    // leaving out time but assuming 8 bytes for encoding, s. below
    info += msg.getConversationID();



    char* buffer = (char*) calloc(1,BUFFER_MAX+1);
    if(!buffer)
    {
        fprintf(stderr, "Allocation of memory for test message failed\n");
        exit(0);
    }
    srand(::time(NULL));
    for(size_t i = 0; i < BUFFER_MAX; ++i)
    {
        buffer[i] = (char) 48 + rand() % 40;
    }

   // memset(buffer, 1, BUFFER_MAX);
    buffer[BUFFER_MAX] = '\0';
    std::string content(buffer, BUFFER_MAX);
    msg.setContent(content);

    uint32_t payloadsize = info.size() + BUFFER_MAX + 8 + 1; // s. comment above for performative and replyBy
    printf("# PAYLOAD SIZE: %d\n", payloadsize);
    printf("# CONTENT SIZE: %lu\n", content.size());
    printf("# MSG CONTENT SIZE: %lu\n", msg.getContent().size());


    MessageParser inputParser;

    std::string encodedMsg;
    printf("#<encoding> <content-size in byte> <encoded-msg-size in bytes > <overhead-percent> <encoding-time in ms/msg> <decoding-time in ms/msg> <epochs>\n");
    for(int i = static_cast<int>(representation::BITEFFICIENT); i < static_cast<int>(representation::END_MARKER); ++i)
    {
        base::Stats<double> msgEncodingStats;
        base::Stats<double> msgDecodingStats;

        base::Stats<double> envEncodingStats;
        base::Stats<double> envDecodingStats;

        representation::Type representationType = static_cast<representation::Type>(i);
        {
            struct timeval start, stop, diff;
            for(int i = 0; i < epochs; ++i)
            {
               gettimeofday(&start, 0);
               encodedMsg = MessageGenerator::create(msg, representationType);
               gettimeofday(&stop, 0);
               timeval_subtract(&diff, &stop, &start);
               double totaltime = diff.tv_sec*1000 + diff.tv_usec/1000.0;
               msgEncodingStats.update(totaltime);
            }
        }
        {
            ACLMessage msg;
            struct timeval start, stop, diff;
            for(int i = 0; i < epochs; ++i)
            {
               gettimeofday(&start, 0);
               if(!inputParser.parseData(encodedMsg, msg, representationType))
               {
                   printf("Could not parse: using %s\n", representation::TypeTxt[representationType].c_str());
               }
               gettimeofday(&stop, 0);
               timeval_subtract(&diff, &stop, &start);
                double totaltime = diff.tv_sec*1000 + diff.tv_usec/1000.0;
                msgDecodingStats.update(totaltime);
            }
        }

        double overheadInPercent = (encodedMsg.size() - payloadsize)*1.0/(1.0*encodedMsg.size());

        printf("message  %d %d %10.6f %10.6f %10.6f %10.6f %10.6f %d %s\n", BUFFER_MAX, (int) encodedMsg.size(), overheadInPercent, msgEncodingStats.mean(), msgEncodingStats.stdev(), msgDecodingStats.mean(), msgDecodingStats.stdev(), epochs, representation::TypeTxt[representationType].c_str());

        // envelope
        //printf("#<encoding> <content-size in byte> <encoded-env-size in bytes > <overhead-percent> <encoding-time in ms/msg> <decoding-time in ms/msg> <epochs>\n");
        std::string encodedEnvelope;
        for(int e = static_cast<int>(representation::BITEFFICIENT); e < static_cast<int>(representation::END_MARKER); ++e)
        {
            if( e == static_cast<int>(representation::STRING_REP))
            {
                continue;
            }

            representation::Type envRepresentationType = static_cast<representation::Type>(e);

            for(int i = 0; i < epochs; ++i)
            {
                {
                    ACLEnvelope env(msg, representationType);
                    struct timeval start, stop, diff;
                    gettimeofday(&start, 0);
                    encodedEnvelope = EnvelopeGenerator::create(env, envRepresentationType);

                    gettimeofday(&stop, 0);
                    timeval_subtract(&diff, &stop, &start);

                    double totaltime = diff.tv_sec*1000 + diff.tv_usec/1000.0;
                    envEncodingStats.update(totaltime);
                }

                ACLEnvelope decodedEnvelope;
                {
                    struct timeval start, stop, diff;
                    gettimeofday(&start, 0);
                    if(!EnvelopeParser::parseData(encodedEnvelope, decodedEnvelope, envRepresentationType))
                    {
                        printf("Decoding envelope failed using: %s\n", representation::TypeTxt[envRepresentationType].c_str());
                        break;
                    }
                    gettimeofday(&stop, 0);
                    timeval_subtract(&diff, &stop, &start);

                    double totaltime = diff.tv_sec*1000 + diff.tv_usec/1000.0;
                    envDecodingStats.update(totaltime);
                }
            }

            overheadInPercent = (encodedEnvelope.size() - encodedMsg.size())/(1.0*encodedEnvelope.size());
            printf("envelope %d %d %10.6f %10.6f %10.6f %10.6f %10.6f %d %s %s\n",  BUFFER_MAX, (int) encodedEnvelope.size(), overheadInPercent, envEncodingStats.mean(), envEncodingStats.stdev(), envDecodingStats.mean(), envDecodingStats.stdev(), epochs, representation::TypeTxt[envRepresentationType].c_str(), representation::TypeTxt[representationType].c_str());

        }
        printf(" ---\n");
    }

    delete buffer;
    return 0;
}
