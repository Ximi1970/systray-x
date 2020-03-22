#!/bin/bash

VERSION=$1

SERVICE_FILE="./dist/rpm/_service"
sed -i -e "s?\(^.*\"revision\">\).*\(</param>\)?\1$VERSION\2?" $SERVICE_FILE
sed -i -e "s?\(^.*\"versionformat\">\).*\(</param>\)?\1$VERSION\2?" $SERVICE_FILE

MANIFEST_FILE="./webext/manifest.json"
sed -i -e "s?\(^.*\"version\": \"\).*\(\",\)?\1$VERSION\2?" $MANIFEST_FILE
