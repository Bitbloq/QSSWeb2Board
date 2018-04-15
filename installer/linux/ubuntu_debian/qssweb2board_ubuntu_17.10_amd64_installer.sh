#!/bin/sh
# script for QSSWeb2Board

#install dependencies
sudo apt-get install qt5-default libqt5serialport5 libqt5websockets5 libqt5network5 libqt5widgets5 libqt5gui5 libqt5core5a

#install qssweb2board
sudo dpkg -i qssweb2board_2.0-1ubuntu17.10_amd64.deb

#adduser to groups
sudo adduser $USER qssweb2board

#adduser to dialout
echo "adding user to group dialout"
sudo adduser $USER dialout
