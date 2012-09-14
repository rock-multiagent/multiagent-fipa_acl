#include "statemachine_factory.h"
#include <boost/filesystem/operations.hpp>
#include <base/logging.h>
#include "statemachine_reader.h"
#include "transition.h"

namespace fs = boost::filesystem;

namespace fipa {
namespace acl {

bool StateMachineFactory::msPreparedResourceDir = false;
std::vector<std::string> StateMachineFactory::msResourceDirs;
std::map<std::string, StateMachine> StateMachineFactory::msStateMachines;

StateMachineReader StateMachineFactory::msStateMachineReader;

void StateMachineFactory::setProtocolResourceDir(const std::string& resourceDir)
{
    msResourceDirs.clear();
    addProtocolResourceDir(resourceDir);
}

void StateMachineFactory::addProtocolResourceDir(const std::string& resourceDir)
{
    fs::path protocolDir = fs::path(resourceDir);
    if(fs::is_directory(protocolDir))
    {
        std::vector<std::string>::iterator resourcesIt = std::find(msResourceDirs.begin(), msResourceDirs.end(), protocolDir.string());
        if(resourcesIt == msResourceDirs.end())
        {
            msResourceDirs.push_back(protocolDir.string());
            msPreparedResourceDir = false;
            LOG_INFO("Add protocol resource dir: '%s'", protocolDir.string().c_str());
        } else {
            LOG_WARN("Protocol resource dir '%s' already added", protocolDir.string().c_str());
        }
    } else {
        std::string msg = "Could not add resource dir: '" + resourceDir + "', since it does not exist";
        throw std::runtime_error(msg);
    }
}

void StateMachineFactory::prepareProtocolsFromResourceDirs()
{
    std::vector<std::string>::const_iterator it = msResourceDirs.begin();
    for(; it != msResourceDirs.end(); ++it)
    {
        prepareProtocolsFromResourceDir(*it);
    }
}

void StateMachineFactory::prepareProtocolsFromResourceDir(const std::string& resourceDir)
{
    fs::path protocolDir = fs::path(resourceDir);
    LOG_INFO("Prepare protocols from: '%s'", protocolDir.string().c_str());
    if(fs::is_directory(protocolDir))
    {
        std::vector<fs::path> files;
        std::copy(fs::directory_iterator(protocolDir), fs::directory_iterator(), std::back_inserter(files));

        std::vector<fs::path>::iterator it = files.begin();
        for(;it != files.end(); ++it)
        {
            if(fs::is_regular_file(*it))
            {
                // Extract protocol name
                std::string protocolName = it->string();
                size_t pos = protocolName.find_last_of("/");
                protocolName.erase(0, pos+1);

                try {
                    // Loading the state machine from the given spec
                    StateMachine statemachine = msStateMachineReader.loadSpecification(it->string()); 
                    LOG_INFO("Register protocol %s", protocolName.c_str());
                    std::map<std::string, StateMachine>::const_iterator statemachinesIt = msStateMachines.find(protocolName);
                    if( statemachinesIt != msStateMachines.end())
                    {
                        LOG_WARN("Protocol '%s' already registered - will use statemachine specification from '%s'", protocolName.c_str(), it->string().c_str());
                    }
                    msStateMachines[protocolName] = statemachine;
                } catch(const std::runtime_error& e)
                {
                    LOG_ERROR("Error loading specification for: '%s' - %s", protocolName.c_str(), e.what());
                }
            }
        }
    }

    msPreparedResourceDir = true;
}

StateMachine StateMachineFactory::getStateMachine(const std::string& protocol)
{
    if(!msPreparedResourceDir)
    {
        StateMachineFactory::prepareProtocolsFromResourceDirs();
    }

    std::map<std::string, StateMachine>::iterator it = msStateMachines.find(protocol);

    if(it != msStateMachines.end())
    {
        return it->second;
    }

    LOG_WARN("StateMachine for protocol '%s' not found", protocol.c_str());
    throw std::runtime_error("State machine for requested protocol does not exist");
}

} // end namespace acl
} // end namespace fipa

