#!/bin/bash


#INSTALL dependencies
sudo apt-get update
sudo apt-get install qtcreator libqt5websockets5-dev libqt5serialport5-dev build-essential zip unzip

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

echo ${OS}
echo ${VER}
echo ${ARCH}

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

echo ${OS}
echo ${VER}
echo ${ARCH}


packageDir=qssweb2board_2.0-1${OS}${VER}_${ARCH}

echo ${packageDir}

#create packageDir form template
rm -fr ${packageDir}
cp -fr qssweb2board_2.0-template ${packageDir}

#Download arduino BQ version

echo "Downloading arduino_BQ_Linux_${ARCH}.zip"
cd ${packageDir}/opt/QSSWeb2Board/res/
wget https://github.com/avalero/QSSWeb2Board/releases/download/18.16.4/arduino1.8.5_BQ_Linux_${ARCH}.zip
unzip arduino_BQ_Linux_${ARCH}.zip
rm arduino_BQ_Linux_${ARCH}.zip
cd -


#build application
baseDir=$(pwd)

mkdir build
cd build

echo "running qmake on ../../../../src/QSSWeb2Board/QSSWeb2Board.pro"
qmake ../../../../src/QSSWeb2Board/QSSWeb2Board.pro
echo "running make..."
make
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



echo sudo dpkg -i ${packageDir}.deb >> installer-${packageDir}.sh

zip -r installer-${packageDir}.zip ${packageDir}.deb installer-${packageDir}.sh


cd ${baseDir}
cd build
make clean
cd ${baseDir}
rm -fr ${packageDir}
