#include "UserdefParam.h"

namespace fipa {

namespace acl {

std::string UserdefParam::getValue() {return value;}

void UserdefParam::setValue(std::string val) {value = val;}

std::string UserdefParam::getName() {return name;}

void UserdefParam::setName (std::string nam) {name = nam;}

}//end of acl namespace

}// end of fipa namespace

