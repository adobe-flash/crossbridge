#!/bin/sh
# Usage: `sh modify_neverball_pkg_uninstaller.sh Uninstall\ Neverball.mpkg`
# Please run this script on the .mpkg installer after building.
distFile=$1/Contents/distribution.dist
tempDistFile=$1/Contents/distribution.dist.temp
packageLoc=$1/Contents/Packages/
sed 's/neverballuninstallscript.pkg</Neverball_Uninstall_Script.pkg</' "$distFile" > "$tempDistFile"
rm "$distFile"
mv "$tempDistFile" "$distFile"
mv "$packageLoc/neverballuninstallscript.pkg" "$packageLoc/Neverball_Uninstall_Script.pkg"