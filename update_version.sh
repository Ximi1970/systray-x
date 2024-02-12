#!/bin/bash

if [[ -z $2 ]] ; then
  RELEASE=1
else
  RELEASE=$2
fi

VERSION=$1

echo $VERSION-$RELEASE

VERSION_AR=(${VERSION//./ })
VERSION_MAJOR=${VERSION_AR[0]}
VERSION_MINOR=${VERSION_AR[1]}
VERSION_BUILD=${VERSION_AR[2]}

SERVICE_FILE="./dist/rpm/kde/_service"
sed -i -e "s?\(^.*\"revision\">\).*\(</param>\)?\1$VERSION\2?" $SERVICE_FILE
sed -i -e "s?\(^.*\"versionformat\">\).*\(</param>\)?\1$VERSION\2?" $SERVICE_FILE

SERVICE_FILE="./dist/rpm/gnome/_service"
sed -i -e "s?\(^.*\"revision\">\).*\(</param>\)?\1$VERSION\2?" $SERVICE_FILE
sed -i -e "s?\(^.*\"versionformat\">\).*\(</param>\)?\1$VERSION\2?" $SERVICE_FILE

SERVICE_FILE="./dist/rpm/minimal/_service"
sed -i -e "s?\(^.*\"revision\">\).*\(</param>\)?\1$VERSION\2?" $SERVICE_FILE
sed -i -e "s?\(^.*\"versionformat\">\).*\(</param>\)?\1$VERSION\2?" $SERVICE_FILE


SERVICE_FILE="./dist/deb/kde/debian.changelog"
sed -i -e "0,/systray-x/{s?\(^systray-x \).*\( unstable.*\)?\1($VERSION-$RELEASE)\2?}" $SERVICE_FILE
SERVICE_FILE="./dist/deb/kde/systray-x.dsc"
sed -i -e "s?\(^Version: \).*?\1$VERSION-$RELEASE?" $SERVICE_FILE

SERVICE_FILE="./dist/deb/gnome/debian.changelog"
sed -i -e "0,/systray-x/{s?\(^systray-x-gnome \).*\( unstable.*\)?\1($VERSION-$RELEASE)\2?}" $SERVICE_FILE
SERVICE_FILE="./dist/deb/gnome/systray-x-gnome.dsc"
sed -i -e "s?\(^Version: \).*?\1$VERSION-$RELEASE?" $SERVICE_FILE

SERVICE_FILE="./dist/deb/minimal/debian.changelog"
sed -i -e "0,/systray-x/{s?\(^systray-x-minimal \).*\( unstable.*\)?\1($VERSION-$RELEASE)\2?}" $SERVICE_FILE
SERVICE_FILE="./dist/deb/minimal/systray-x-minimal.dsc"
sed -i -e "s?\(^Version: \).*?\1$VERSION-$RELEASE?" $SERVICE_FILE


SERVICE_FILE="./dist/arch/kde/PKGBUILD"
sed -i -e "s?\(^pkgver=\).*?\1$VERSION?" $SERVICE_FILE
sed -i -e "s?\(^pkgrel=\).*?\1$RELEASE?" $SERVICE_FILE

SERVICE_FILE="./dist/arch/gnome/PKGBUILD"
sed -i -e "s?\(^pkgver=\).*?\1$VERSION?" $SERVICE_FILE
sed -i -e "s?\(^pkgrel=\).*?\1$RELEASE?" $SERVICE_FILE

SERVICE_FILE="./dist/arch/minimal/PKGBUILD"
sed -i -e "s?\(^pkgver=\).*?\1$VERSION?" $SERVICE_FILE
sed -i -e "s?\(^pkgrel=\).*?\1$RELEASE?" $SERVICE_FILE


MANIFEST_FILE="./webext/manifest.json"
sed -i -e "s?\(^.*\"version\": \"\).*\(\",\)?\1$VERSION\2?" $MANIFEST_FILE

NSIS32_FILE="./dist/SysTray-X-32bit.nsi"
sed -i -e "s?\(^!define VERSIONMAJOR \).*?\1$VERSION_MAJOR?" $NSIS32_FILE
sed -i -e "s?\(^!define VERSIONMINOR \).*?\1$VERSION_MINOR?" $NSIS32_FILE
sed -i -e "s?\(^!define VERSIONBUILD \).*?\1$VERSION_BUILD?" $NSIS32_FILE

NSIS64_FILE="./dist/SysTray-X-64bit.nsi"
sed -i -e "s?\(^!define VERSIONMAJOR \).*?\1$VERSION_MAJOR?" $NSIS64_FILE
sed -i -e "s?\(^!define VERSIONMINOR \).*?\1$VERSION_MINOR?" $NSIS64_FILE
sed -i -e "s?\(^!define VERSIONBUILD \).*?\1$VERSION_BUILD?" $NSIS64_FILE

BUILD_NUMBER=`git rev-list --count HEAD`
GIT_HASH=`git rev-parse HEAD`
GIT_BRANCH=`git rev-parse --abbrev-ref HEAD`

echo "VERSION=${VERSION}" > dist/rpm/VERSION
echo "BUILD_NUMBER=${BUILD_NUMBER}" >> dist/rpm/VERSION
echo "GIT_HASH=${GIT_HASH}" >> dist/rpm/VERSION
echo "GIT_BRANCH=${GIT_BRANCH}" >> dist/rpm/VERSION
