#!/bin/bash 

# Token set in .gitconfig

VERSION=`git describe --long | sed "s/-.*//"`

./ghr ${VERSION} bin/
