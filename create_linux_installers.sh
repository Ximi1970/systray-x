
#!/bin/bash

VERSION=`git describe --long | sed "s/-.*//"`

OBS_BASE="https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons"
OBS_PACKAGE="systray-x"

OBS_RPM_ARCHS="openSUSE_Leap_15.1/x86_64 "
OBS_RPM_PKS="_ "
OBS_RPM_ARCHS+="openSUSE_Leap_15.2/x86_64 "
OBS_RPM_PKS+="_ "
OBS_RPM_ARCHS+="openSUSE_Tumbleweed/i586 "
OBS_RPM_PKS+="tmblwd "
OBS_RPM_ARCHS+="openSUSE_Tumbleweed/x86_64 "
OBS_RPM_PKS+="tmblwd "
OBS_RPM_ARCHS+="SLE_15/x86_64 "
OBS_RPM_PKS+="sle150 "
OBS_RPM_ARCHS+="SLE_15_SP1/x86_64 "
OBS_RPM_PKS+="sle151 "

OBS_DEB_ARCHS="xUbuntu_18.04/i386 "
OBS_DEB_PKS="bionic1804 "
OBS_DEB_ARCHS+="xUbuntu_18.04/amd64 "
OBS_DEB_PKS+="bionic1804 "
OBS_DEB_ARCHS+="xUbuntu_19.04/amd64 "
OBS_DEB_PKS+="disco1904 "
OBS_DEB_ARCHS+="xUbuntu_19.10/amd64 "
OBS_DEB_PKS+="focal1910 "

mkdir -p bin
pushd bin > /dev/null 2>&1

#
# Create bash installers for RPM based distributions
#
INDEX=1
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

  rpm2cpio $RPM_FILE | cpio -i --to-stdout ./usr/share/doc/packages/systray-x/systray-x@Ximi1970.xpi > systray-x@Ximi1970.xpi 2>/dev/null

  if [ ! -f ../systray-x@Ximi1970.xpi ] ; then
    cp -f systray-x@Ximi1970.xpi ..
  fi

  #
  # Get JSON
  #
  cp -f ../../app/config/linux/SysTray_X.json.template SysTray_X.json.template

  #
  # Cleanup
  #
  rm -f index.html
  mv -f $RPM_FILE ..

  tar cJf ../$PACKAGE_NAME.tar.xz .

  popd > /dev/null 2>&1

  #
  # Rename package
  #
  PACKAGE_EXT=`echo $OBS_RPM_PKS | cut -d' ' -f$INDEX`
  if [ "$PACKAGE_EXT" != "_" ] ; then
    NEW_RPM_FILE=`echo $RPM_FILE | sed -s "s/\($OBS_PACKAGE-$VERSION-\)\(.*\)/\1$PACKAGE_EXT\.\2/"`
    mv -f $RPM_FILE $NEW_RPM_FILE
  fi

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

  #
  # Update index
  #
  INDEX=$((INDEX+1))
done


#
# Create bash installers for DEB based distributions
#
INDEX=1
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

  dpkg --fsys-tarfile $DEB_FILE | tar xOf - ./usr/lib/thunderbird-addons/extensions/systray-x@Ximi1970.xpi > systray-x@Ximi1970.xpi

  #
  # Get JSON
  #
  cp -f ../../app/config/linux/SysTray_X.json.template SysTray_X.json.template

  #
  # Cleanup
  #
  rm -f index.html
  mv -f $DEB_FILE ..

  tar cJf ../$PACKAGE_NAME.tar.xz .

  popd > /dev/null 2>&1

  #
  # Rename package
  #
  PACKAGE_EXT=`echo $OBS_DEB_PKS | cut -d' ' -f$INDEX`
  if [ "$PACKAGE_EXT" != "_" ] ; then
    NEW_DEB_FILE=`echo $DEB_FILE | sed -s "s/\($OBS_PACKAGE\_$VERSION\_\)\(.*\)/\1$PACKAGE_EXT\_\2/"`
    mv -f $DEB_FILE $NEW_DEB_FILE
  fi

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

  #
  # Update index
  #
  INDEX=$((INDEX+1))
done 

popd > /dev/null 2>&1
