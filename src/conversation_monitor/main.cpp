#include <iostream>
#include <fipa_acl/conversation_monitor.h>

using namespace fipa::acl;

int main(int argc, char** argv)
{
	StateMachineBuilder builder = StateMachineBuilder();
	StateMachine request = builder.loadSpecification(std::string("request"));
	request.print();
	
	//std::cout<<"random output\n";

	return 0;
}




