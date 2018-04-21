#!/bin/bash


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

if [ -z "${1}" ];then
	echo "No OS introduced"
else
	OS=${1}
fi

if [ -z "${2}" ];then
	echo "No VERSION introduced"
else
	VER=${2}
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


packageDir=qssweb2board_2.0-1${OS}${VER}_${ARCH}

#create packageDir form template
rm -fr ${packageDir}
cp -fr qssweb2board_2.0-template ${packageDir}

#Download arduino BQ version

echo "Downloading arduino_BQ_Linux_${ARCH}.zip"
cd ${packageDir}/opt/QSSWeb2Board/res/
wget https://github.com/avalero/QSSWeb2Board/releases/download/18.16.4/arduino1.8.5_BQ_Linux_${ARCH}.zip
unzip arduino1.8.5_BQ_Linux_${ARCH}.zip > /dev/null
rm arduino1.8.5_BQ_Linux_${ARCH}.zip > /dev/null
cd -


#build application
baseDir=$(pwd)

if [ -d build ]; then
  rm -fr build
fi

mkdir build
cd build > /dev/null

echo "running qmake on ../../../../src/QSSWeb2Board/QSSWeb2Board.pro"
qmake ../../../../src/QSSWeb2Board/QSSWeb2Board.pro > /dev/null
echo "running make..."
make > /dev/null
cd ${baseDir}

#copy application into packageDir
cp build/QSSWeb2Board ${packageDir}/opt/QSSWeb2Board/

sed -i -e "s/###ARCH###/${ARCH}/g" ${packageDir}/DEBIAN/control

#build deb package
dpkg --build ${packageDir}

#create install script

cp install-template.sh installer-${packageDir}.sh
sed -i -e "s/###OS###/${OS}/g" installer-${packageDir}.sh
sed -i -e "s/###VERSION###/${VER}/g" installer-${packageDir}.sh
sed -i -e "s/###ARCH###/${BITS}/g" installer-${packageDir}.sh

sed -i -e "s/###INSTALL_COMMAND###/sudo dpkg -i ${packageDir}.deb/g" installer-${packageDir}.sh

zip -r installer-${packageDir}.zip ${packageDir}.deb installer-${packageDir}.sh


cd ${baseDir}
cd build
make clean
cd ${baseDir}
rm -fr build
rm -fr ${packageDir}
