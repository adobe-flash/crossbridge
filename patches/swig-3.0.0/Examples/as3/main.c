#include <stdlib.h>
#include <stdio.h>
#include <AS3/AS3.h>


int main (int argc, char **argv) {
    setlinebuf(stdout);
    inline_as3(
      "import SWIGExample.*;\n"
      "ASMain();\n"
    );
    return 0;
}

__attribute__((annotate("as3sig:public function Trace(s:String):void"),
    annotate("as3package:SWIGExample")))
void Trace() {
    inline_as3("import com.adobe.flascc.CModule;\n");
    inline_as3("s = s + '\\n';\n");
    inline_as3("var ptr = CModule.alloca(s.length);\n");
    inline_as3("CModule.writeString(ptr, s);\n");
    inline_as3("var fake_errno_ptr = CModule.alloca(4);\n");
    inline_as3("CModule.kernel.write(1, ptr, s.length, fake_errno_ptr);"); 
}

