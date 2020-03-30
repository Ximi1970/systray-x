#!/bin/bash 

VERSION=`git describe --long | sed "s/-.*//"`

./ghr ${VERSION} bin/
