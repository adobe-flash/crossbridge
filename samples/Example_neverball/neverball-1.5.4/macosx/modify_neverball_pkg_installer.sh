#!/bin/sh
# Usage: `sh modify_neverball_pkg_installer.sh Neverball-v1.5.x.mpkg`
# Please run this script on the .mpkg installer after building.
# PackageMaker doesn't seem to generate suitable sub-package names.
# This changes the .pkg 'receipts' to have meaningful names (Neverball_Docs.pkg vs. docs.pkg).
distFile=$1/Contents/distribution.dist
tempDistFile=$1/Contents/distribution.dist.temp
packageLoc=$1/Contents/Packages/
sed 's/neverball.pkg</Neverball.pkg</' "$distFile" > "$tempDistFile"
rm "$distFile"
mv "$tempDistFile" "$distFile"
sed 's/neverputt.pkg</Neverputt.pkg</' "$distFile" > "$tempDistFile"
rm "$distFile"
mv "$tempDistFile" "$distFile"
sed 's/data.pkg</Neverball_Data.pkg</' "$distFile" > "$tempDistFile"
rm "$distFile"
mv "$tempDistFile" "$distFile"
sed 's/docs.pkg</Neverball_Docs.pkg</' "$distFile" > "$tempDistFile"
rm "$distFile"
mv "$tempDistFile" "$distFile"
sed 's/uninstallNeverball.pkg</Neverball_Uninstaller.pkg</' "$distFile" > "$tempDistFile"
rm "$distFile"
mv "$tempDistFile" "$distFile"
sed 's/neverballfolder.pkg</Neverball_Icon.pkg</' "$distFile" > "$tempDistFile"
rm "$distFile"
mv "$tempDistFile" "$distFile"
sed 's/seticon.pkg</Neverball_Seticon.pkg</' "$distFile" > "$tempDistFile"
rm "$distFile"
mv "$tempDistFile" "$distFile"
mv "$packageLoc/neverball.pkg" "$packageLoc/Neverball.pkg"
mv "$packageLoc/neverputt.pkg" "$packageLoc/Neverputt.pkg"
mv "$packageLoc/data.pkg" "$packageLoc/Neverball_Data.pkg"
mv "$packageLoc/docs.pkg" "$packageLoc/Neverball_Docs.pkg"
mv "$packageLoc/uninstallNeverball.pkg" "$packageLoc/Neverball_Uninstaller.pkg"
mv "$packageLoc/neverballfolder.pkg" "$packageLoc/Neverball_Icon.pkg"
mv "$packageLoc/seticon.pkg" "$packageLoc/Neverball_Seticon.pkg"
