#include "UserdefParam.h"

std::string UserdefParam::getValue() {return value;}

void UserdefParam::setValue(std::string& val) {value = val;}

std::string UserdefParam::getName() {return name;}

void UserdefParam::setName (std::string& nam) {name = nam;}
