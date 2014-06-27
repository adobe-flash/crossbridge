                                       // Chapter 11 - Program 4
#include <iostream>
#include "supervsr.h"
using namespace std;

// In all cases, init_data assigns values to the class variables and
//  display outputs the values to the monitor for inspection.

supervisor::supervisor(char *in_name, int in_salary, char *in_title)
{
   name = in_name;
   salary = in_salary;
   title= in_title;
}




void
supervisor::display(void)
{
   cout << "Supervisor --> " << name << "'s salary is " << salary <<
                                 " and is the " << title << ".\n\n";
}




programmer::programmer(char *in_name, int in_salary, char *in_title,
                  char *in_language)
{
   name = in_name;
   salary = in_salary;
   title = in_title;
   language = in_language;
}




void
programmer::display(void)
{
   cout << "Programmer --> " << name << "'s salary is " << salary <<
                                        " and is " << title << ".\n";
   cout << "               " << name << "'s specialty is " <<
                                                 language << ".\n\n";
}




secretary::secretary(char *in_name, int in_salary,
                             int in_shorthand, int in_typing_speed)
{
   name = in_name;
   salary = in_salary;
   shorthand = in_shorthand;
   typing_speed = in_typing_speed;
}




void
secretary::display(void)
{
   cout << "Secretary ---> " << name << "'s salary is " << salary <<
                                                                 ".\n";
   cout << "               " << name << " types " << typing_speed <<
              " per minute and can ";
   if (!shorthand) cout << "not ";
   cout << "take shorthand.\n\n";
}

consultant::consultant(char *in_name, 
		      int in_salary,
		      char *in_specialty, 
		      int in_contract_length)
{
   name = in_name;
   salary = in_salary;
   specialty = in_specialty;
   contract_length = in_contract_length;
}




void
consultant::display(void)
{
   cout << "Consultant --> " << name << "'s salary is " << salary <<
                                      " and consults in " <<
					  specialty << ".\n"; 
   cout << "               " << name << "'s contract lasts " <<
                                                 contract_length << 
						   " weeks.\n\n"; 
}

                                     // Chapter 11 - Program 5

person *staff1,*staff2,*staff3,*staff4;

int main()
{
supervisor *suppt;
programmer *progpt;
secretary *secpt;

   suppt = new supervisor("Big John", 5100, "President");
   staff1 = suppt;

   progpt = new programmer("Joe Hacker", 3500, "debugger", "Pascal");
   staff2 = progpt;

   progpt = new programmer("OOP Wizard", 7700, "senior analyst", "C++");
   staff3 = progpt;

   secpt = new secretary("Tillie Typer", 2200, 1, 85);
   staff4 = secpt;

   staff1->display();
   staff2->display();
   staff3->display();
   staff4->display();
}




// Result of execution

// XYZ Staff -- note salary is monthly.
//
// Supervisor --> Big John's salary is 5100 and is the President.
//
// Programmer --> Joe Hacker's salary is 3500 and is debugger.
//                Joe Hacker's specialty is Pascal.
//
// Programmer --> OOP Wizard's salary is 7700 and is senior analyst.
//                OOP Wizard's specialty is C++.
//
// Secretary ---> Tillie Typer's salary is 2200.
//                Tillie typer types 85 per minute and can take shorthand.
//
// Supervisor --> Tom Talker's salary is 5430 and is the sales manager.
//
// Programmer --> Dave Debugger's salary is 5725 and is code maintainer.
//                Dave Debugger's specialty is assembly language.
//
// End of employee list.
