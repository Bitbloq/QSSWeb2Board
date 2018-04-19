#!/bin/sh
# script for QSSWeb2Board

echo "Vas a instalar Web2Board en tu sistema".
echo "Has escogido realizar la instalación para ###OS### ###VERSION### ###ARCH### (amd64 -> 64 bits | i386 -> 32 bits)"
while true; do
    read -p "¿Es este tu sistema operativo? (SI/NO)" yn
    case $yn in
        [SI]* ) break;;
        [NO]* ) exit;;
        * ) echo "Por favor, contesta SI o NO";;
    esac
done


#create qssweb2board group and add user
#sudo addgroup qssweb2board
#sudo adduser $USER qssweb2board

#adduser to dialout
#echo "adding user to group dialout"
#sudo adduser $USER dialout

#install dependencies
sudo apt-get install qt5-default libqt5serialport5 libqt5websockets5 libqt5network5 libqt5widgets5 libqt5gui5 libqt5core5a
