#!/bin/bash

if [ -z "${1}" ];then
	echo "PLEASE INTRODUCE QSSWeb2Board VERSION"
    exit
else
	version=${1}
  echo ${version}
fi

if [ -z "${2}" ];then
	echo "PLEASE INTRODUCE OS (docker name type ubuntu)"
    exit
else
	os=${2}
  echo ${os}
fi

if [ -z "${3}" ];then
	echo "PLEASE INTRODUCE OS version (docker name type 18.04)"
    exit
else
	osv=${3}
  echo ${osv}
fi

if [ -z "${4}" ];then
	echo "PLEASE INTRODUCE ARCHITECTURE (i386 or amd64)"
    exit
else
	arch=${4}
  echo ${arch}
fi
docker run --rm -v $(pwd):/QSSWeb2Board ${os}:${osv} ./QSSWeb2Board/createInstaller.sh ${version} ${os} ${osv} ${arch}
