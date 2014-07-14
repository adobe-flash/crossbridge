#!/bin/sh
# This script reads a .desktop file from stdin and prints the same file, with
# up-to-date translations, to stdout.

while read line; do
    # Remove translations
    if echo $line | grep '^.*\[.*\]=' > /dev/null; then
        continue
    fi

    echo $line

    # Generate translations if current line is translatable
    if echo $line | grep '^Comment=' > /dev/null; then
        msgid=`echo $line | sed 's/^Comment=//'`
        for i in po/*.po; do
            lang=`basename $i | sed 's/\.po//'`
            msgstr=`msgattrib --translated --no-obsolete --no-fuzzy $i \
                    | msggrep --no-location --no-wrap --msgid -F -e "$msgid" \
                    | tail -n 1 | sed 's/^msgstr "\(.*\)"$/\1/'`
            if [ "$msgstr" != "" ]; then
                echo "Comment[$lang]=$msgstr"
            fi
        done
    fi
done
