// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

// parse some XML!

#include <string.h>
#include <stdio.h>
#include <AS3/AS3.h>
#include <expat.h>

// current XML element depth
static int sDepth = 0;

// called at XML element start
static void XMLCALL start(void *data, const char *elem, const char **attr)
{
  int i;

  // indent
  for(i = 0; i < sDepth; i++)
    printf("\t");

  // print element name
  printf("%s:", elem);

  // print attributes
  for(i = 0; attr[i]; i += 2)
    printf(" %s=\"%s\"", attr[i], attr[i + 1]);
  printf("\n");

  // register increased depth
  sDepth++;
}

// called at XML element end
static void XMLCALL end(void *data, const char *elem)
{
  // register decreased depth
  sDepth--;
}

int main()
{
  // create a new XML parser
  XML_Parser parser = XML_ParserCreate(NULL);
  if(!parser)
  {
    fprintf(stderr, "Couldn't create XML parser\n");
    exit(-1);
  }

  // attach our callbacks
  XML_SetElementHandler(parser, start, end);

  // declare an AS3 var named xml of type XML
  AS3_DeclareVar(xml, XML);
  // put some xml in it using describeType on the AS3 Number class closure object
  inline_as3(
      "import flash.utils.describeType\n"
      "xml = describeType(Number)\n"
  );

  // convert the xml to a char *
  char *xmlBytes;
  AS3_MallocString(xmlBytes, xml);
  int len = strlen(xmlBytes);

  // parse it!
  if(XML_Parse(parser, xmlBytes, len, 1) == XML_STATUS_ERROR)
  {
    fprintf(stderr, "Couldn't parse XML at line %d: %s\n",
        XML_GetCurrentLineNumber(parser),
        XML_ErrorString(XML_GetErrorCode(parser)));
    exit(-1);
  }

  // free the bytes
  free(xmlBytes);

  // free the parser
  XML_ParserFree(parser);
  return 0;
}

