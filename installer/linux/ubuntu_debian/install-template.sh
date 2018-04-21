#!/bin/sh
# script for QSSWeb2Board

echo "Vas a instalar Web2Board en tu sistema".
echo "Has escogido realizar la instalación para ###OS### ###VERSION### ###ARCH### bits"
while true; do
    read -p "¿Es este tu sistema operativo? (SI/NO)" yn
    case $yn in
        [SI]* ) break;;
        [NO]* ) exit;;
        * ) echo "Por favor, contesta SI o NO";;
    esac
done

#install dependencies
sudo apt-get install qt5-default libqt5serialport5 libqt5websockets5 libqt5network5 libqt5widgets5 libqt5gui5 libqt5core5a

###INSTALL_COMMAND###

#CLEAR SCREEN
printf "\033c"
echo "====== INSTALACIÓN FINALIZADA ======"
echo "Si desea que otros usuarios usen Bitbloq deberán ser añadidos a los grupos qssweb2board y dialout"
echo "adduser USUARIO qssweb2board"
echo "adduser USUARIO dialout"
echo "===================================="
echo "Por favor, reincie el sistema"
