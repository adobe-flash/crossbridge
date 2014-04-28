#!/bin/sh

LC_ALL=C
export LC_ALL

svn_version()
{
    svn_rev="$(svnversion . /svn/neverball/trunk | tr : +)"
    test "$svn_rev" != "exported" && echo "r$svn_rev"
}

version="$(svn_version || date -u +"%Y-%m-%d")"
test -n "$version" && echo "$version" || echo "unknown"

