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
