#!/bin/bash

VERSION=$1

SERVICE_FILE="./dist/rpm/kde/systray-x.spec"
sed -i -e "s?\(^Requires:       thunderbird <= \).*?\1$VERSION?" $SERVICE_FILE
sed -i -e "s?\(^Requires:       MozillaThunderbird <= \).*?\1$VERSION?" $SERVICE_FILE

SERVICE_FILE="./dist/rpm/gnome/systray-x-gnome.spec"
sed -i -e "s?\(^Requires:       thunderbird <= \).*?\1$VERSION?" $SERVICE_FILE
sed -i -e "s?\(^Requires:       MozillaThunderbird <= \).*?\1$VERSION?" $SERVICE_FILE

SERVICE_FILE="./dist/rpm/minimal/systray-x-minimal.spec"
sed -i -e "s?\(^Requires:       thunderbird <= \).*?\1$VERSION?" $SERVICE_FILE
sed -i -e "s?\(^Requires:       MozillaThunderbird <= \).*?\1$VERSION?" $SERVICE_FILE

SERVICE_FILE="./dist/deb/kde/debian.control"
sed -i -e "s?\(^Depends:.*, thunderbird (<= 1:\).*?\1$VERSION)?" $SERVICE_FILE

SERVICE_FILE="./dist/deb/gnome/debian.control"
sed -i -e "s?\(^Depends:.*, thunderbird (<= 1:\).*?\1$VERSION)?" $SERVICE_FILE

SERVICE_FILE="./dist/deb/minimal/debian.control"
sed -i -e "s?\(^Depends:.*, thunderbird (<= 1:\).*?\1$VERSION)?" $SERVICE_FILE

SERVICE_FILE="./dist/arch/kde/PKGBUILD"
sed -i -e "s?\(^.*thunderbird<=\).*?\1$VERSION'?" $SERVICE_FILE

SERVICE_FILE="./dist/arch/gnome/PKGBUILD"
sed -i -e "s?\(^.*thunderbird<=\).*?\1$VERSION'?" $SERVICE_FILE

SERVICE_FILE="./dist/arch/minimal/PKGBUILD"
sed -i -e "s?\(^.*thunderbird<=\).*?\1$VERSION'?" $SERVICE_FILE

MANIFEST_FILE="./webext/manifest.json"
sed -i -e "s?\(^.*\"strict_max_version\": \"\).*\(\.\*\"\)?\1$VERSION\2?" $MANIFEST_FILE
