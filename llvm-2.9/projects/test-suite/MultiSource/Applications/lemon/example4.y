/* Copyright (GPL) 2004 mchirico@users.sourceforge.net or mchirico@comcast.net
  Simple lemon parser  example.

  
    $ ./lemon example2.y                          

  

*/

%include {   
#include <iostream>  
#include "ex4def.h"
#include "example4.h"


  void token_destructor(Token t)
    {
      std::cout << "In token_destructor t.value= " << t.value << std::endl;
      std::cout << "In token_destructor t.n= " << t.n << std::endl;
    }


}  


%token_type {Token}
%default_type {Token}
%token_destructor { token_destructor($$); }

%type expr {Token}
%type NUM {Token}



   
%left PLUS MINUS.   
%left DIVIDE TIMES.  
   
%parse_accept {
  printf("parsing complete!\n\n\n");
}

   
%syntax_error {  
  std::cout << "Syntax error!" << std::endl;  
}   
   
/*  This is to terminate with a new line */
main ::= in.
in ::= .
in ::= in state NEWLINE.



state ::= expr(A).   { 
                        std::cout << "Result.value=" << A.value << std::endl; 
                        std::cout << "Result.n=" << A.n << std::endl; 

                         }  



expr(A) ::= expr(B) MINUS  expr(C).   { A.value = B.value - C.value; 
                                       A.n = B.n+1  + C.n+1;
                                      }  

expr(A) ::= expr(B) PLUS  expr(C).   { A.value = B.value + C.value; 
                                       A.n = B.n+1  + C.n+1;
                                      }  

expr(A) ::= expr(B) TIMES  expr(C).   { A.value = B.value * C.value;
                                        A.n = B.n+1  + C.n+1;

                                         }  
expr(A) ::= expr(B) DIVIDE expr(C).  { 

         if(C.value != 0){
           A.value = B.value / C.value;
           A.n = B.n+1 + C.n+1;
          }else{
           std::cout << "divide by zero" << std::endl;
           }
}  /* end of DIVIDE */
expr(A) ::= NUM(B). { A.value = B.value; A.n = B.n+1; }


