/**
 *
 * \file UserdefParam.h
 * \author Mircea Cretu Stancu
 * \brief defines the UserdefParam class
 * 
 * \version 1.0
 *  - has very basic features, only what was needed for the ACLMessage and AgentID classes
 *  - may need to be expanded/derived from depending on future needs
 */

#ifndef UserdefMessageParam_H_
#define UserdefMessageParam_H_

#include <string>

namespace fipa {

namespace acl {

    /**
	  \brief overloaded equality operator for UserdefParam class objects 
    */
        //extern bool operator== (UserdefParam &a,UserdefParam &b);
        
/**
    \brief Implements the general User-defined parameters, which are present throughout the fipa specifications(FIPA at http://www.fipa.org).
*/
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
	    UserdefParam();
	   
	    /**
		\brief overloaded copy-constructor
	    */
	    UserdefParam(const UserdefParam &copy);
	    /**
		\brief overloaded assignment operator
	    */
	    UserdefParam& operator=(const UserdefParam& copy);
	    /** \brief setter and getter methods */
              std::string getValue() const;
              void setValue(std::string val);
              std::string getName() const;
              /**
		\brief the method checks whether the passed name string is a word or not(according to the fipa spec)
		\return 0 if successful 1 otherwise(name is un-alterred)
	      */
	    int setName (std::string nam);
};

extern bool operator== (const UserdefParam &a,const UserdefParam &b);

}//end of acl namespace

}// end of fipa namespace
#endif
