// unit.cc -- This is a test case for the dejagnu.h classes.
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009,
// 2010 Free Software Foundation, Inc.

// This file is part of DejaGnu.

// DejaGnu is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.

// DejaGnu is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with DejaGnu; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.

#include <sys/types.h>
#include <iostream>
#include <regex.h>
#include <string>
#include <fstream>
#include <sstream>
#include <set>
#include <dejagnu.h>

using namespace std;

TestState runtest;
TestState test;

class TestClass1
{
public:
  string tname;
  unsigned int tnum;
};

TestClass1 testClass1, testClass2, testClass3;
TestClass1 *testClassPtr;

// We have to do this silly crap with renaming the output string, so
// the generic Tcl code that looks for the output state gets confused,
// and records random duplicate messages.
const char *os1[] = {
    "FAI: ", "PAS: ", "UNT: ", "UNR: "
};

const char *os2[] = {
    "FAILED: ", "PASSED: ", "UNTESTED: ", "UNRESOLVED: "
};

int
main (int argc, char *argv[])
{
  regex_t regex_pat;
  outstate = os1;
  stringstream strbuf;
  streambuf *pbuf;

  // Replace the output buffer for cout, so we can examine it to see
  // what was displayed. Otherwise, there is no way we can test the
  // logging functions completely.
  pbuf = cout.rdbuf ();

  testClass1.tname = "testType1";
  testClass1.tnum = 1;
  testClass2.tname = "testType2";
  testClass2.tnum = 2;
  testClass3.tname = "testType3";
  testClass3.tnum = 3;
  
  // Test the pass message.
  cout.rdbuf (strbuf.rdbuf ());
  strbuf.str ("");
  test.pass ("bogus pass message for testing");
  outstate = os2;
  cout.rdbuf (pbuf);
  if (strncmp (strbuf.str().c_str(), "\tPAS: bogus pass message", 22) == 0)
    runtest.pass ("Pass message");
  else
    runtest.fail ("Pass message");
  
  // Test the fail message.
  cout.rdbuf (strbuf.rdbuf ());
  strbuf.str ("");
  outstate = os1;
  test.fail ("bogus fail message for testing");
  cout.rdbuf (pbuf);
  outstate = os2;
  if (strncmp (strbuf.str().c_str(), "\tFAI: bogus fail message", 22) == 0)
    runtest.pass ("Fail message");
  else
    runtest.fail ("Fail message");
  
  // Test the untested message.
  cout.rdbuf (strbuf.rdbuf ());
  strbuf.str ("");
  outstate = os1;
  test.untested ("bogus untested message for testing");
  cout.rdbuf (pbuf);
  outstate = os2;
  if (strncmp (strbuf.str().c_str(), "\tUNT: bogus untested message", 21) == 0) {
    runtest.pass ("Untested message");
  } else {
    runtest.fail ("Untested message");
  }
  
  // Test the unresolved message.
  cout.rdbuf (strbuf.rdbuf ());
  strbuf.str ("");
  outstate = os1;
  test.unresolved ("bogus unresolved message for testing");
  cout.rdbuf (pbuf);
  outstate = os2;
  if (strncmp (strbuf.str().c_str(), "\tUNR: bogus unresolved message", 21) == 0)
    runtest.pass ("Unresolved message");
  else
    runtest.fail ("Unresolved message");
  
  // Make sure we got everything in the totals.
  cout.rdbuf (strbuf.rdbuf ());
  strbuf.str ("");
  regcomp (&regex_pat,
	   "\t#passed.*#real failed.*#untested.*#unresolved",
	   REG_NOSUB);

  test.totals ();
  cout.rdbuf (pbuf);
  if (regexec (&regex_pat, strbuf.str().c_str(), 0, (regmatch_t *) 0, 0) == 0)
    runtest.pass ("Totals message");
  else
    runtest.fail ("Totals message");

  return 0;
}

