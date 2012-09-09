/**
 * \file userdef_param.h
 * \author Mircea Cretu Stancu, Thomas Roehr
 * \brief defines the UserdefParam class
 * 
 * \version 1.0
 *  - has very basic features, only what was needed for the ACLMessage and AgentID classes
 *  - may need to be expanded/derived from depending on future needs
 */

#ifndef FIPA_ACL_USERDEF_MESSAGEPARAM_H
#define FIPA_ACL_USERDEF_MESSAGEPARAM_H

#include <string>

namespace fipa {

namespace acl {

/**
\brief overloaded equality operator for UserdefParam class objects 
*/
        
/**
\brief Implements the general User-defined parameters, which are present throughout the fipa specifications(FIPA at http://www.fipa.org).
*/
class UserdefParam {
       
/*
As the specification states, the value of any user defined parameter is a string; so if other data types are needed they are 
to be parsed to a string if need for more complex objects as parameter value arises, they are either to be serialized(or parsed 
somehow similar to the bit-efficient parsing), or we can customize the speciffication and use a template or overload it 
*/
private:
    std::string mName;
    std::string mValue;
        
public:

    /**
     * Default constructor
     */
    UserdefParam();

    /**
     * Constructor for a userdefined parameter of a given name
     * \param name Name of the userdefined parameter
     */
    UserdefParam(const std::string& name);

    /**
     * Assignment operator
     */
    UserdefParam& operator=(const UserdefParam& other);

    /**
     * Comparision operator
     */
    bool operator==(const UserdefParam& other) const;
   
    /* \brief setter and getter methods */
    /**
     * Get value of the userdefined parameter
     * \return value of the userdefined parameter
     */
    std::string getValue() const { return mValue; }
    
    /**
     * Set the value associated with the userdefined parameter
     * \param val Value of the userdefined parameter
     */
    void setValue(const std::string& value) { mValue = value; }

    /**
     * Get the name of the userdefined parameter
     * \return name
     */
    std::string getName() const { return mName; }

    /**
     * \brief the method checks whether the passed name string is a word or not(according to the fipa spec)
     * \param name Name to set for the userdefined parameter
     * \return true if successful and false otherwise(name is un-alterred)
     */
    bool setName(const std::string& name);

};

}//end of acl namespace
}// end of fipa namespace
#endif
