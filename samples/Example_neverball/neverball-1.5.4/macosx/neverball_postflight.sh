#!/bin/sh
sudo /seticon -d /neverball_folder.icns /Library/Application\ Support/Neverball\ Data/ ~/.neverball ~/.neverball-dev
sudo rm -fr /seticon /neverball_folder.icns /Library/Receipts/Neverball_Seticon.pkg /Library/Receipts/Neverball_Icon.pkg /Library/Receipts/Neverball_Uninstaller.pkg
sudo mv /Library/Application\ Support/Neverball\ Data/Uninstall\ Neverball.mpkg ~/
sudo chmod 777 ~/Uninstall\ Neverball.mpkg