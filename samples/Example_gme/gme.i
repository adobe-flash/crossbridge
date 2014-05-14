
%module libgme

%{
#include "AS3/AS3.h"
#include "gme/gme.h"
#include "demo/Wave_Writer.h"
%}

// Ignore this because it is not defined
%ignore gme_clear_playlist;

// Specify a custom typemape for gme_err_t, that maps NULL (no error) to
// undefined in ActionScript
%typemap(out) gme_err_t {
    AS3_DeclareVar($result, String);
    if ($1) {
        int len = strlen($1);
        AS3_CopyCStringToVar($result, $1, len);
    } else {
       swig_as3("result = null;\n");
    }
}

%include "gme/gme.h"
%include "demo/Wave_Writer.h"

