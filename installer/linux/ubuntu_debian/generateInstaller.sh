#!/bin/bash

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
    ;;
i*86)
    ARCH=i386  # or IA32 or Intel32 or whatever
    ;;
*)
    # leave ARCH as-is
    ;;
esac

echo ${OS}
echo ${VER}
echo ${ARCH}


packageDir=qssweb2board_2.0-1${OS}${VER}_${ARCH}

echo ${packageDir}

#create packageDir form template
rm -fr ${packageDir}
cp -fr qssweb2board_2.0-template ${packageDir}


#build application
baseDir=$(pwd)

mkdir build
cd build

echo "running qmake on " $1
qmake $1
echo "running make..."
make
cd ${baseDir}

#copy application into packageDir
cp build/QSSWeb2Board ${packageDir}/opt/QSSWeb2Board/

#build deb package
dpkg --build ${packageDir}

#create install script

cp install-template.sh installer-${packageDir}.sh

echo sudo dpkg -i ${packageDir}.deb >> installer-${packageDir}.sh

zip -r installer-${packageDir}.zip ${packageDir}.deb installer-${packageDir}.sh


cd ${baseDir}
cd build
make clean
cd ${baseDir}
