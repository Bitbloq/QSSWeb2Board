#!/bin/bash

if [ -z "${1}" ];then
	echo "PLEASE INTRODUCE VERSION"
    exit
else
	version=${1}
fi

#INSTALL dependencies

echo "Instalando dependencias..."
sudo apt-get update > /dev/null
sudo apt-get install qt5-qmake qt5-default libqt5websockets5-dev libqt5serialport5-dev build-essential zip unzip

#GET VERSION AND NAME OF OS
if [ -f /etc/os-release ]; then
    # freedesktop.org and systemd
    . /etc/os-release
    OS=$NAME
    VER=$VERSION_ID
elif type lsb_release >/dev/null 2>&1; then
    # linuxbase.org
    OS=$(lsb_release -si)
    VER=$(lsb_release -sr)
elif [ -f /etc/lsb-release ]; then
    # For some versions of Debian/Ubuntu without lsb_release command
    . /etc/lsb-release
    OS=$DISTRIB_ID
    VER=$DISTRIB_RELEASE
elif [ -f /etc/debian_version ]; then
    # Older Debian/Ubuntu/etc.
    OS=Debian
    VER=$(cat /etc/debian_version)
elif [ -f /etc/SuSe-release ]; then
    # Older SuSE/etc.
    ...
elif [ -f /etc/redhat-release ]; then
    # Older Red Hat, CentOS, etc.
    ...
else
    # Fall back to uname, e.g. "Linux <version>", also works for BSD, etc.
    OS=$(uname -s)
    VER=$(uname -r)
fi

case $(uname -m) in
x86_64)
    ARCH=amd64  # or AMD64 or Intel64 or whatever
    BITS='64'
    ;;
i*86)
    ARCH=i386  # or IA32 or Intel32 or whatever
    BITS='32'
    ;;
*)
    # leave ARCH as-is
    ;;
esac

if [ -z "${2}" ];then
	echo "No OS introduced"
else
	OS=${2}
fi

if [ -z "${3}" ];then
	echo "No VERSION introduced"
else
	VER=${3}
fi

echo "Creando instalador para ${OS} ${VER} ${ARCH}"
while true; do
    read -p "¿Es este tu sistema operativo? (SI/NO)" yn
    case $yn in
        [SI]* ) break;;
        [NO]* ) exit;;
        * ) echo "Por favor, contesta SI o NO";;
    esac
done


packageDir=qssweb2board_${version}${OS}${VER}_${ARCH}

#create packageDir form template
rm -fr ${packageDir}
cp -fr qssweb2board_2.0-template ${packageDir}

#Download arduino BQ version

echo "Downloading arduino_BQ_Linux_${ARCH}.zip"
cd ${packageDir}/opt/QSSWeb2Board/res/
wget https://github.com/bitbloq/QSSWeb2Board/releases/download/18.07.17/arduino1.8.5_BQ_Linux_${ARCH}.zip
unzip arduino1.8.5_BQ_Linux_${ARCH}.zip > /dev/null
rm arduino1.8.5_BQ_Linux_${ARCH}.zip > /dev/null
cd -

#copy knownBoards
cp knownBoards.json ${packageDir}/opt/QSSWeb2Board/res/arduino/libraries/

#build application
baseDir=$(pwd)

if [ -d build ]; then
  rm -fr build
fi

mkdir build
cd build > /dev/null

echo "running qmake on ../../src/QSSWeb2Board/QSSWeb2Board.pro"
qmake ../../src/QSSWeb2Board/QSSWeb2Board.pro
echo "running make..."
make 
cd ${baseDir}

#copy application into packageDir
cp build/QSSWeb2Board ${packageDir}/opt/QSSWeb2Board/

sed -i -e "s/###ARCH###/${ARCH}/g" ${packageDir}/DEBIAN/control
sed -i -e "s/###VERSION###/${version}/g" ${packageDir}/DEBIAN/control

cp ./config/config-template.xml ./config/config.xml
sed -i -e "s/###VERSION###/${version}/g" ./config/config.xml


echo "Creating script installer..."
#build deb package
dpkg --build ${packageDir}

#mv package to deb subdirectory
if [ -d deb ]; then
  rm -fr deb
fi

mkdir deb
mv ${packageDir}.deb ./deb/

#create install script

cp install-template.sh installer-${packageDir}.sh
sed -i -e "s/###OS###/${OS}/g" installer-${packageDir}.sh
sed -i -e "s/###VERSION###/${VER}/g" installls
er-${packageDir}.sh
sed -i -e "s/###ARCH###/${BITS}/g" installer-${packageDir}.sh

sed -i -e "s/###INSTALL_COMMAND###/sudo gdebi --non-interactive \${mydir}\/deb\/${packageDir}.deb/g" installer-${packageDir}.sh

cp install-template-gui.sh gui-installer-${packageDir}.sh
sed -i -e "s/###OS###/${OS}/g" gui-installer-${packageDir}.sh
sed -i -e "s/###VERSION###/${VER}/g" gui-installer-${packageDir}.sh
sed -i -e "s/###ARCH###/${BITS}/g" gui-installer-${packageDir}.sh

sed -i -e "s/###INSTALL_COMMAND###/sudo gdebi --non-interactive \${mydir}\/deb\/${packageDir}.deb/g" gui-installer-${packageDir}.sh

cp ./packages/com.bq.qssweb2board/meta/installscript-template.js ./packages/com.bq.qssweb2board/meta/installscript.js
sed -i -e "s/###GUI_INSTALLER###/gui-installer-${packageDir}.sh/g" ./packages/com.bq.qssweb2board/meta/installscript.js

zip -r installer-${packageDir}.zip ./deb/${packageDir}.deb installer-${packageDir}.sh

mv ./deb ./packages/com.bq.qssweb2board/data 
mv gui-installer-${packageDir}.sh ./packages/com.bq.qssweb2board/data

echo "Creating GUI installer..."
binarycreator -c ./config/config.xml -p packages graphical-installer-${packageDir}
chmod a+x graphical-installer-${packageDir}


#remove all temp files

rm installer-${packageDir}.sh
cd ${baseDir}
cd build
make clean
cd ${baseDir}
rm -fr build
rm -fr ${packageDir}
rm -fr ./packages/com.bq.qssweb2board/data/*
rm ./packages/com.bq.qssweb2board/meta/installscript.js
rm ./config/config.xml
