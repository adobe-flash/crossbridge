/* File : example.c */

class base { 
protected: 
virtual ~base(){} 
}; 

class derived : public base { 
public: 
virtual ~derived() {} 
};
