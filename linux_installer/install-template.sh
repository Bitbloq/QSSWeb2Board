#!/bin/sh
# script for QSSWeb2Board

echo "Vas a instalar Web2Board en tu sistema".
echo "Has escogido realizar la instalación para ###OS### ###VERSION### ###ARCH### bits"
while true; do
    read -p "¿Es este tu sistema operativo? (SI/NO) " yn
    case $yn in
        [SI]* ) break;;
        [NO]* ) exit;;
        * ) echo "Por favor, contesta SI o NO ";;
    esac
done

mydir="${0%/*}"

#install dependencies
sudo apt-get update

sudo apt-get install gdebi

###INSTALL_COMMAND###

#CLEAR SCREEN
#printf "\033c"
echo ""
echo ""
echo ""
echo "====== INSTALACIÓN FINALIZADA ======"
echo "COMPRUEBE QUE NO HAY NINGÚN MENSAJE DE ERROR"
echo "===================================="
echo "===================================="
echo "Si desea que otros usuarios usen Bitbloq deberán ser añadidos a los grupos qssweb2board y dialout"
echo "adduser USUARIO qssweb2board"
echo "adduser USUARIO dialout"
echo "===================================="
echo "Por favor, reincie el sistema"
