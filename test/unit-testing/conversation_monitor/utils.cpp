#include "utils.h"
#include <stdexcept>

std::string getProtocolPath()
{
    char buffer[1024];
    if( readlink("/proc/self/exe", buffer, 1024) == -1)
    {
        throw std::runtime_error("Could not find process: self");
    }
    std::string str(buffer);
    std::string executionDir = str.substr(0, str.rfind('/'));
    // Assuming we have do a build into build/ parallel to src/ 
    std::string configurationPath = executionDir + "/../../../../configuration/protocols";
    return configurationPath;
}
