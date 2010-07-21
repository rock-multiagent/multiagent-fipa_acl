#include <iostream>
#include "debug.h"



int main()
{

fipa::acl::MessageParameter mp;
mp.name = "test";

std::cout << mp << std::endl;
return 0;
}
