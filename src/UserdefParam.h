/**
 *
 * \file UserdefParam.h
 * \author Mircea Cretu Stancu
 * \brief Implements the general User-defined parameters, which are present throughout the fipa specifications(FIPA at http://www.fipa.org).
 * 
 * \version 0.1
 *  - has very basic features, only what was needed for the ACLMessage and AgentAID classes
 *  - may need to be expanded/derived from depending on future needs
 */

#ifndef UserdefMessageParam_H_
#define UserdefMessageParam_H_

#include<string>

//template<typename T>
class UserdefParam {
       
       private:
               std::string name;
               std::string value;
               
       public:
              std::string getValue();
              void setValue(std::string val);
              std::string getName();
              void setName (std::string nam);
};

#endif
