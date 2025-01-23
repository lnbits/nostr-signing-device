#!/bin/sh
tag=$1

rm -rf dist
mkdir -p dist/$tag

cp build/snsd.ino.bin dist/$tag
cp build/snsd.ino.partitions.bin dist/$tag
cp build/boot_app0.bin dist/$tag
cp build/bootloader.bin dist/$tag

cp build/manifest.template.json dist/$tag/manifest.json
sed -i -e "s/%version%/$tag/g" dist/$tag/manifest.json
