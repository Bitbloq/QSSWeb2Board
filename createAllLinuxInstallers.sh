#!/bin/bash

./build_installer_universal.sh 3.5.0 ubuntu 19.10 amd64
./build_installer_universal.sh 3.5.0 ubuntu 19.04 amd64
./build_installer_universal.sh 3.5.0 ubuntu 18.10 amd64
./build_installer_universal.sh 3.5.0 ubuntu 18.04 amd64
./build_installer_universal.sh 3.5.0 ubuntu 16.04 amd64
./build_installer_universal.sh 3.5.0 i386/ubuntu 16.04 i386
./build_installer_universal.sh 3.5.0 debian 10 amd64
./build_installer_universal.sh 3.5.0 debian 9 amd64
./build_installer_universal.sh 3.5.0 i386/debian 9 i386
./build_installer_universal.sh 3.5.0 i386/debian 10 i386
