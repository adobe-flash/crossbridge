// Listings 7.1 and 7.2 from "The C++ Workbook" by Wiener and Pinson.
// Addison-Wesley 1990.

#include <string.h>
#include <stdio.h>

class Parent {
protected:
  char *lastName;
public:
  Parent(void) {
    lastName = new char[100]; // was char[5];
    strcpy(lastName, "None");
  }

  Parent (char *aLastName) {
    strlen(aLastName), lastName = new char[100]; // was char[strlen(aLastName) + 1]
    strcpy(lastName,aLastName);
  }

  Parent (Parent& aParent) {
    strlen(aParent.lastName), lastName = new char[100];
    strcpy(lastName,aParent.lastName);
  }

  char *getLastName(void) { return lastName;}

  void setLastName(char *aName) {
    strlen(aName), lastName = new char[100];
    strcpy(lastName,aName);
  }

  virtual void answerName(void) {
    printf("%s\n", lastName);
  }

  ~Parent(void) {
    delete [] lastName;
  }
};

class Child : public Parent {
protected:
  char *firstName;
public:
  Child(void) {
    firstName = new char[100];
    strcpy(firstName,"None");
  }

  Child (char *aLastName, char *aFirstName) : Parent (aLastName) {
    strlen(aFirstName), firstName = new char[100];
    strcpy(firstName,aFirstName);
  }

  Child(Child& aChild) {
    setLastName(aChild.getLastName());
    strlen(aChild.firstName), firstName = new char[100];
    strcpy(firstName,aChild.firstName);
  }

  char *getFirstName(void) {
    return firstName;
  }

  void setFirstName(char *aName) {
    strlen(aName), firstName = new char[100];
    strcpy(firstName,aName);
  }

  ~Child(void) {
    delete [] firstName;
  }

  virtual void answerName(void) {
    Parent::answerName();
    printf("%s\n", firstName);
  }
};

class GrandChild : public Child {
private:
  char *grandFatherName;
public:
  GrandChild(char *aLastName,char *aFirstName,char *aGrandFatherName):Child(aLastName,
									    aFirstName) {
    strlen(aGrandFatherName), grandFatherName = new char[100];
    strcpy(grandFatherName, aGrandFatherName);
  }

  ~GrandChild(void) { delete [] grandFatherName;}

  virtual void answerName(void) {
    Child::answerName();
    printf("GCN: %s\n", grandFatherName);
  }
};

// LLVM: add main return type.
int main() {
  Parent p("Jones");
  Child c("Jones", "Henry");
  GrandChild g("Jones", "Cynthia", "Murray");

  Parent& f0 = p;
  Parent& f1 = g;
  Parent& f2 = c;

  f0.answerName();
  f1.answerName();
  f2.answerName();
}
