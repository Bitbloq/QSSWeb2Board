#!/bin/bash

if [ -z "${1}" ];then
	echo "PLEASE INTRODUCE VERSION"
    exit
else
	version=${1}
fi


#INSTALL dependencies

echo "Instalando dependencias..."
apt update
apt upgrade -y
apt dist-upgrade -y
apt autoremove -y
apt install gdebi qt5-qmake qt5-default libqt5websockets5-dev libqt5serialport5-dev build-essential zip unzip -y

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


#packageDir=qssweb2board_${version}${OS}${VER}_${ARCH}
packageDir=QSSWeb2BoardOnlineCompiler

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

#build application
baseDir=$(pwd)

if [ -d build ]; then
  rm -fr build
fi

mkdir build
cd build > /dev/null

echo "running qmake on ../../src/QSSWeb2Board/QSSWeb2Board.pro"
qmake ../../src/QSSWeb2Board/QSSWeb2Board.pro "ONLINE_COMPILER=true"
echo "running make..."
make 
cd ${baseDir}

#copy application into packageDir
cp build/QSSWeb2Board ${packageDir}/opt/QSSWeb2Board/

sed -i -e "s/###ARCH###/${ARCH}/g" ${packageDir}/DEBIAN/control
sed -i -e "s/###VERSION###/${version}/g" ${packageDir}/DEBIAN/control

#build deb package
dpkg --build ${packageDir}

#mv package to deb subdirectory
if [ -d deb ]; then
  rm -fr deb
fi

mv ${packageDir}.deb

echo "Installing Online Compiler".

gdebi --non-interactive QSSWeb2BoardOnlineCompiler.deb

echo "Removing Temp files"
#remove all temp files

cd ${baseDir}
cd build
make clean
cd ${baseDir}
rm -fr build
rm -fr ${packageDir}
rm -fr ${packageDir}.deb