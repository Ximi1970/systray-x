#!/bin/bash

VERSION=$1

SERVICE_FILE="./dist/rpm/kde/_service"
sed -i -e "s?\(^.*\"revision\">\).*\(</param>\)?\1$VERSION\2?" $SERVICE_FILE
sed -i -e "s?\(^.*\"versionformat\">\).*\(</param>\)?\1$VERSION\2?" $SERVICE_FILE

SERVICE_FILE="./dist/rpm/gnome/_service"
sed -i -e "s?\(^.*\"revision\">\).*\(</param>\)?\1$VERSION\2?" $SERVICE_FILE
sed -i -e "s?\(^.*\"versionformat\">\).*\(</param>\)?\1$VERSION\2?" $SERVICE_FILE

SERVICE_FILE="./dist/rpm/minimal/_service"
sed -i -e "s?\(^.*\"revision\">\).*\(</param>\)?\1$VERSION\2?" $SERVICE_FILE
sed -i -e "s?\(^.*\"versionformat\">\).*\(</param>\)?\1$VERSION\2?" $SERVICE_FILE

MANIFEST_FILE="./webext/manifest.json"
sed -i -e "s?\(^.*\"version\": \"\).*\(\",\)?\1$VERSION\2?" $MANIFEST_FILE

BUILD_NUMBER=`git rev-list --count HEAD`
GIT_HASH=`git rev-parse HEAD`
GIT_BRANCH=`git rev-parse --abbrev-ref HEAD`

echo "VERSION=${VERSION}" > dist/rpm/VERSION
echo "BUILD_NUMBER=${BUILD_NUMBER}" >> dist/rpm/VERSION
echo "GIT_HASH=${GIT_HASH}" >> dist/rpm/VERSION
echo "GIT_BRANCH=${GIT_BRANCH}" >> dist/rpm/VERSION
