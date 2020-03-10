#!/bin/bash

VERSION=`git describe --long | sed "s/-.*//"`

OBS_BASE="https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons"
OBS_PACKAGE="systray-x"

OBS_RPM_ARCHS="openSUSE_Leap_15.1/x86_64 "
OBS_RPM_ARCHS="openSUSE_Leap_15.2/x86_64 "
OBS_RPM_ARCHS+="openSUSE_Tumbleweed/i586 "
OBS_RPM_ARCHS+="openSUSE_Tumbleweed/x86_64 "
OBS_RPM_ARCHS+="SLE_15/x86_64 "
OBS_RPM_ARCHS+="SLE_15_SP1/x86_64 "

OBS_DEB_ARCHS="xUbuntu_18.04/i386 "
OBS_DEB_ARCHS+="xUbuntu_18.04/amd64 "
OBS_DEB_ARCHS+="xUbuntu_19.04/amd64 "
OBS_DEB_ARCHS+="xUbuntu_19.10/amd64 "

mkdir -p bin
pushd bin > /dev/null 2>&1

#
# Create bash installers for RPM based distributions
#
for rpmdir in $OBS_RPM_ARCHS ; do
  #
  # Get base name
  #
  BASE_NAME=${rpmdir//'/'/$'-'}
  
  PACKAGE_NAME=SysTray-X-$VERSION-$BASE_NAME
  
  rm -rf $PACKAGE_NAME
  mkdir -p $PACKAGE_NAME
  pushd $PACKAGE_NAME > /dev/null 2>&1
  
  #
  # Get index.html
  #
  wget -q "$OBS_BASE/$rpmdir/"

  #
  # Find rpm
  #
  RPM_FILE=`grep ">systray.*<" index.html | sed -e "s/.*>\(systray-x.*rpm\)<.*/\1/"`
  
  #
  # Get rpm
  #
  wget -q "$OBS_BASE/$rpmdir/$RPM_FILE"
  
  #
  # Extract 
  #
  rpm2cpio $RPM_FILE | cpio -i --to-stdout ./usr/bin/SysTray-X > SysTray-X 2>/dev/null
  chmod 755 SysTray-X

  #
  # Get JSON
  #
  cp -f ../../app/config/linux/SysTray_X.json.template SysTray_X.json.template
  
  #
  # Cleanup
  #
  rm -f index.html
  rm -f $RPM_FILE

  tar cJf ../$PACKAGE_NAME.tar.xz .
  
  popd > /dev/null 2>&1

  #
  # Create installer
  #
  cp -f ../dist/install.sh SysTray-X-$VERSION-$BASE_NAME-install.sh
  cat $PACKAGE_NAME.tar.xz >> SysTray-X-$VERSION-$BASE_NAME-install.sh
  chmod 755 SysTray-X-$VERSION-$BASE_NAME-install.sh
  
  #
  # Cleanup
  #
  rm -rf $PACKAGE_NAME
  rm -f $PACKAGE_NAME.tar.xz
  
done


#
# Create bash installers for DEB based distributions
#
for debdir in $OBS_DEB_ARCHS ; do
  #
  # Get base name
  #
  BASE_NAME=${debdir//'/'/$'-'}
  
  PACKAGE_NAME=SysTray-X-$VERSION-$BASE_NAME
  
  rm -rf $PACKAGE_NAME
  mkdir -p $PACKAGE_NAME
  pushd $PACKAGE_NAME > /dev/null 2>&1
  
  #
  # Get index.html
  #
  wget -q "$OBS_BASE/$debdir/"

  #
  # Find deb
  #
  DEB_FILE=`grep ">systray.*\.deb<" index.html | sed -e "s/.*>\(systray-x.*deb\)<.*/\1/"`
  
  #
  # Get deb
  #
  wget -q "$OBS_BASE/$debdir/$DEB_FILE"
  
  #
  # Extract 
  #
  dpkg --fsys-tarfile $DEB_FILE | tar xOf - ./usr/bin/SysTray-X > SysTray-X
  chmod 755 SysTray-X

  #
  # Get JSON
  #
  cp -f ../../app/config/linux/SysTray_X.json.template SysTray_X.json.template
  
  #
  # Cleanup
  #
  rm -f index.html
  rm -f $DEB_FILE

  tar cJf ../$PACKAGE_NAME.tar.xz .
  
  popd > /dev/null 2>&1

  #
  # Create installer
  #
  cp -f ../dist/install.sh SysTray-X-$VERSION-$BASE_NAME-install.sh
  cat $PACKAGE_NAME.tar.xz >> SysTray-X-$VERSION-$BASE_NAME-install.sh
  chmod 755 SysTray-X-$VERSION-$BASE_NAME-install.sh
  
  #
  # Cleanup
  #
  rm -rf $PACKAGE_NAME
  rm -f $PACKAGE_NAME.tar.xz
  
done 

popd > /dev/null 2>&1
