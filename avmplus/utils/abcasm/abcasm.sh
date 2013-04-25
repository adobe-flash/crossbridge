#!/bin/sh
# -*- Mode: Java; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- 
# vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) 
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed

ABCASM_HOME=`dirname $0`
ANTLR_HOME=$ABCASM_HOME/../../other-licenses/ANTLR
ANTLR_RUNTIME=$ANTLR_HOME/antlr-runtime-3.3.jar
JAVA=java

# enable setting asc.jar using -a switch
while getopts "j:a:" OPTION
    do
        case $OPTION in
            j) JAVA="$OPTARG" ;;
            a) JAVAARGS="$OPTARG" ;;
        esac
    done
shift $(($OPTIND - 1))
    
echo "$*"
case `uname -s` in
	CYGWIN*)
		# Classpath set for Cygwin systems
		"$JAVA" $JAVAARGS -ea -classpath $ABCASM_HOME/classes\;$ABCASM_HOME/lib/aet.jar\;$ABCASM_HOME/lib/abcasm.jar\;$ANTLR_RUNTIME com.adobe.flash.abcasm.ABCasm $*
		;;
	*)
		# 'NIX systems use this classpath
		"$JAVA" $JAVAARGS -ea -classpath $ABCASM_HOME/classes:$ABCASM_HOME/lib/aet.jar:$ABCASM_HOME/lib/abcasm.jar:$ANTLR_RUNTIME com.adobe.flash.abcasm.ABCasm $*
		;;
esac

# the other way to invoke abcasm is with the jar
#java -ea  -jar $ABCASM_HOME/lib/abcasm.jar $*
