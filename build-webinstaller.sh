#!/bin/sh
PROJECT_NAME=snsd
RELEASE=https://github.com/lnbits/nostr-signing-device/releases/download
INSTALLER_REPO=https://github.com/lnbits/hardware-installer
INSTALLER_PATH=./hardware-installer/public/firmware

git clone $INSTALLER_REPO
cd hardware-installer
git checkout before_boot
cd ..

cp INSTALLER.md ./hardware-installer/public/INSTALLER.md
cp versions.json ./hardware-installer/src/versions.json
cp config.js ./hardware-installer/src/config.js

sed -i "s/%title%/$PROJECT_NAME/g" ./hardware-installer/index.html

cp ./configure/nsd_config.html ./hardware-installer/config.html

mkdir -p $INSTALLER_PATH
for device in $(jq -r '.devices[]' ./hardware-installer/src/versions.json); do
    for version in $(jq -r '.versions[]' ./hardware-installer/src/versions.json); do
        mkdir -p $INSTALLER_PATH/$device/$version
        wget $RELEASE/$version/$PROJECT_NAME.ino.bin
        wget $RELEASE/$version/$PROJECT_NAME.ino.partitions.bin
        wget $RELEASE/$version/$PROJECT_NAME.ino.bootloader.bin
        mv $PROJECT_NAME.ino.bin $INSTALLER_PATH/$device/$version
        mv $PROJECT_NAME.ino.partitions.bin $INSTALLER_PATH/$device/$version
        mv $PROJECT_NAME.ino.bootloader.bin $INSTALLER_PATH/$device/$version
    done
done
