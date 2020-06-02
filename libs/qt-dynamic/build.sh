#!/bin/bash

QT_DOWNLOAD=https://download.qt.io/archive/qt/5.14/5.14.2/single/qt-everywhere-src-5.14.2.tar.xz
QT_PACKAGE=${QT_DOWNLOAD##*/}
QT_DIR=${QT_PACKAGE%.t*}

if [ ! -f ${QT_PACKAGE} ] ; then
  wget -q ${QT_DOWNLOAD}
fi

if [ ! -d ${QT_DIR} ] ; then
  tar xvJf ${QT_PACKAGE}
fi

ARCH=`uname -m`
if [ "${ARCH}" == 'x86_64' ]; then
  LIBDIR=lib64
else
  LIBDIR=lib
fi

PREFIX=`pwd`/../../app/rootfs/usr
mkdir -p ${PREFIX}

mkdir -p ${QT_DIR}/build
pushd ${QT_DIR}/build > /dev/null 2>&1

#-no-rpath
#./configure LDFLAGS=-Wl,-rpath=$ORIGIN/path/to/library

../configure -opensource -confirm-license -prefix ${PREFIX}/Qt-opensource-5.14.2 -libdir ${PREFIX}/Qt-opensource-5.14.2/${LIBDIR} -fontconfig -sql-sqlite -no-sql-odbc -system-freetype -qt-zlib -qt-libpng -qt-libjpeg -openssl-linked -qt-xcb -verbose --recheck-all -no-compile-examples -nomake tests -nomake examples -no-opengl -no-feature-concurrent -no-feature-xml -no-feature-testlib -skip qt3d -skip qtactiveqt -skip qtandroidextras -skip qtcanvas3d -skip qtcharts -skip qtconnectivity -skip qtdatavis3d -skip qtdeclarative -skip qtdoc -skip qtgamepad -skip qtgraphicaleffects -skip qtimageformats -skip qtlocation -skip qtmacextras -skip qtmultimedia -skip qtnetworkauth -skip qtpurchasing -skip qtquickcontrols -skip qtquickcontrols2 -skip qtremoteobjects -skip qtscript -skip qtscxml -skip qtsensors -skip qtserialbus -skip qtserialport -skip qtsvg -skip qtspeech -skip qttools -skip qttranslations -skip qtvirtualkeyboard -skip qtwayland -skip qtwebchannel -skip qtwebsockets -skip qtwebview -skip qtwinextras -skip qtx11extras -skip qtxmlpatterns -skip qtwebengine
make -j
make install

popd > /dev/null 2>&1
