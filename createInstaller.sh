#!/bin/bash

if [ -z "${1}" ];then
	echo "PLEASE INTRODUCE QSSWeb2Board VERSION"
    exit
else
	version=${1}
fi

if [ -z "${2}" ];then
	echo "PLEASE INTRODUCE OS (docker name type ubuntu)"
    exit
else
	os=${2}
fi

if [ -z "${3}" ];then
	echo "PLEASE INTRODUCE OS version (docker name type 18.04)"
    exit
else
	osv=${3}
fi

if [ -z "${4}" ];then
	echo "PLEASE INTRODUCE ARCHITECTURE (i386 or amd64)"
    exit
else
	arch=${3}
fi

cd QSSWeb2Board/linux_installer
./generateInstaller.sh ${version} ${os} ${osv} ${arch}
