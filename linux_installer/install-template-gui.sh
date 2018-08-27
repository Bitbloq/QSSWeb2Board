#!/bin/sh
# script for QSSWeb2Board

#install dependencies
sudo apt-get update

sudo apt-get install gdebi

mydir="${0%/*}"

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
