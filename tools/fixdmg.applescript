tell application "Finder"
 tell disk "Crossbridge 1.0"
       open
       set current view of container window to icon view
       set toolbar visible of container window to false
       set statusbar visible of container window to false
       set the bounds of container window to {400, 100, 885, 430}
       set theViewOptions to the icon view options of container window
       set arrangement of theViewOptions to not arranged
       set icon size of theViewOptions to 64
       update without registering applications
       delay 5
       eject
 end tell
end tell
