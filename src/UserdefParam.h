/**
 *
 * \file UserdefParam.h
 * \author Mircea Cretu Stancu
 * \brief Implements the general User-defined parameters, which are present throughout the fipa specifications(FIPA at http://www.fipa.org).
 * 
 * \version 1.0
 *  - has very basic features, only what was needed for the ACLMessage and AgentAID classes
 *  - may need to be expanded/derived from depending on future needs
 */

#ifndef UserdefMessageParam_H_
#define UserdefMessageParam_H_

#include <string>

namespace fipa {

namespace acl {

    //extern bool operator== (UserdefParam &a, UserdefParam &b);

class UserdefParam {
       
/*
	as the specification states, the value of any user defined parameter is a string; so if other data types are needed they are to be parsed to a string
	if need for more complex objects as parameter value arises, they are either to be serialized(or parsed somehow simmilar to the bit-efficient parsing), or we can customize the speciffication and use a template or overload it

*/
       private:
               std::string name;
               std::string value;
               
       public:
/*
	setter and getter methods so far
*/
              std::string getValue();
              void setValue(std::string val);
              std::string getName();
              void setName (std::string nam);
};


}//end of acl namespace

}// end of fipa namespace
#endif
