#!/bin/bash

VERSION="0~git.*"

OBS_REPO_BASE="https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons:/Staging:/Test"
TARGET_DIR="bin-test"

GNOME_APPINDICATOR="https://github.com/ubuntu/gnome-shell-extension-appindicator.git"

OBS_PACKAGE="systray-x"

OBS_RPM_ARCHS=""
OBS_RPM_PKS=""
OBS_RPM_GNOME_EXT=""
OBS_RPM_ARCHS+="openSUSE_Leap_15.1/x86_64 "
OBS_RPM_PKS+="_ "
OBS_RPM_GNOME_EXT+="26 "
OBS_RPM_ARCHS+="openSUSE_Leap_15.2/x86_64 "
OBS_RPM_PKS+="_ "
OBS_RPM_GNOME_EXT+="26 "
OBS_RPM_ARCHS+="openSUSE_Tumbleweed/i586 "
OBS_RPM_PKS+="tmblwd "
OBS_RPM_GNOME_EXT+="26 "
OBS_RPM_ARCHS+="openSUSE_Tumbleweed/x86_64 "
OBS_RPM_PKS+="tmblwd "
OBS_RPM_GNOME_EXT+="26 "
OBS_RPM_ARCHS+="SLE_15/x86_64 "
OBS_RPM_PKS+="sle150 "
OBS_RPM_GNOME_EXT+="26 "
OBS_RPM_ARCHS+="SLE_15_SP1/x86_64 "
OBS_RPM_PKS+="sle151 "
OBS_RPM_GNOME_EXT+="26 "
OBS_RPM_ARCHS+="Fedora_31/x86_64 "
OBS_RPM_PKS+="fed31 "
OBS_RPM_GNOME_EXT+="26 "
OBS_RPM_ARCHS+="Fedora_32/x86_64 "
OBS_RPM_PKS+="fed32 "
OBS_RPM_GNOME_EXT+="33 "


OBS_DEB_ARCHS=""
OBS_DEB_PKS=""
OBS_DEB_GNOME_EXT=""
OBS_DEB_ARCHS+="Debian_10/i386 "
OBS_DEB_PKS+="deb10 "
OBS_DEB_GNOME_EXT+="26 "
OBS_DEB_ARCHS+="Debian_10/amd64 "
OBS_DEB_PKS+="deb10 "
OBS_DEB_GNOME_EXT+="26 "
OBS_DEB_ARCHS+="xUbuntu_16.04/i386 "
OBS_DEB_PKS+="xenial1604 "
OBS_DEB_GNOME_EXT+="26 "
OBS_DEB_ARCHS+="xUbuntu_16.04/amd64 "
OBS_DEB_PKS+="xenial1604 "
OBS_DEB_GNOME_EXT+="26 "
OBS_DEB_ARCHS+="xUbuntu_18.04/i386 "
OBS_DEB_PKS+="bionic1804 "
OBS_DEB_GNOME_EXT+="26 "
OBS_DEB_ARCHS+="xUbuntu_18.04/amd64 "
OBS_DEB_PKS+="bionic1804 "
OBS_DEB_GNOME_EXT+="26 "
OBS_DEB_ARCHS+="xUbuntu_19.04/amd64 "
OBS_DEB_PKS+="disco1904 "
OBS_DEB_GNOME_EXT+="26 "
OBS_DEB_ARCHS+="xUbuntu_19.10/amd64 "
OBS_DEB_PKS+="eoan1910 "
OBS_DEB_GNOME_EXT+="26 "
OBS_DEB_ARCHS+="xUbuntu_20.04/amd64 "
OBS_DEB_PKS+="focal2004 "
OBS_DEB_GNOME_EXT+="0 "


create_gnome_extension_tar() {

  local GIT_BASE=$1
  local GNOME_EXT=$2

  ##########################################
  #
  # Create user installable
  # gnome-shell-extension-appindicator 
  #
  ##########################################
  
  if [ -f gnome-shell-extension-${GNOME_EXT}.tar.xz ] ; then
    return
  fi
  
  if [ ${GNOME_EXT} != "26" ] && [ ${GNOME_EXT} != "33" ] ; then
    return
  fi
  
  git clone -q ${GIT_BASE}
  pushd gnome-shell-extension-appindicator > /dev/null 2>&1
  git checkout -q v${GNOME_EXT}
  popd > /dev/null 2>&1
  
  #
  # Create tar
  #
  mv -f gnome-shell-extension-appindicator appindicatorsupport@rgcjonas.gmail.com
  tar -cJf gnome-shell-extension-${GNOME_EXT}.tar.xz appindicatorsupport@rgcjonas.gmail.com

  #
  # Cleanup
  #
  rm -rf appindicatorsupport@rgcjonas.gmail.com
}


create_rpm_tar() {

  local REPO_BASE=$1
  local REPO_DISTR=$2
  local REPO_ARCH=$3
  local RPM_NAME_EXT=$4
  local GNOME_EXT=$5

  ##########################################
  #
  # Create the SysTray-X tar
  #
  ##########################################

  #
  # Get index.html
  #
  rm -f index.html
  wget -q "${REPO_BASE}/${REPO_DISTR}/${REPO_ARCH}/"
  
  #
  # Find rpm
  #
  local RPM_FILE=$(grep ">systray-x-[^d].*<" index.html | sed -e "s/.*>\(systray-x-[^d].*rpm\)<.*/\1/")
  rm -f index.html

  echo "Found: "${RPM_FILE}
  
  FOUND_VERSION=$(echo ${RPM_FILE} | sed -e "s/systray-x-\(.*\)-.*/\1/")

  echo "Version: "${FOUND_VERSION}

  #
  # Create tar dir
  #
  local TAR_DIR=SysTray-X-${FOUND_VERSION}-${REPO_DISTR}

  rm -rf ${TAR_DIR}
  mkdir -p ${TAR_DIR}
    
  #
  # Add the gnome extension to the tar
  #
  if [ "${GNOME_EXT}" == "26" ] && [ -f gnome-shell-extension-26.tar.xz ] ; then
    cp -f gnome-shell-extension-26.tar.xz ${TAR_DIR}/gnome-shell-extension.tar.xz
  fi
  if [ "${GNOME_EXT}" == "33" ] && [ -f gnome-shell-extension-33.tar.xz ] ; then
    cp -f gnome-shell-extension-33.tar.xz ${TAR_DIR}/gnome-shell-extension.tar.xz
  fi
  
  #
  # Get the SysTray-X rpm
  #
  wget -q "${REPO_BASE}/${REPO_DISTR}/${REPO_ARCH}/${RPM_FILE}"

  #
  # Get compression type
  #
  COMPRESSION=$(rpm -qp --qf '%{PAYLOADCOMPRESSOR}\n' ${RPM_FILE})
#  echo ${COMPRESSION}
  
  case ${COMPRESSION} in
    zstd)
      #
      # Extract 
      #
      rpm2cpio ${RPM_FILE} | zstd -d | cpio --quiet -idm

      #
      # Get files
      #
      cp -f ./usr/bin/SysTray-X ./${TAR_DIR}/SysTray-X
      chmod 755 ./${TAR_DIR}/SysTray-X
      cp -f ./usr/share/doc/systray-x/systray-x@Ximi1970.xpi ./${TAR_DIR}/systray-x@Ximi1970.xpi
      ;;
    *)
      #
      # Extract 
      #
      rpm2cpio ${RPM_FILE} | cpio --quiet -idm

      #
      # Get files
      #
      cp -f ./usr/bin/SysTray-X ./${TAR_DIR}/SysTray-X
      chmod 755 ./${TAR_DIR}/SysTray-X
      cp -f ./usr/share/doc/packages/systray-x/systray-x@Ximi1970.xpi ./${TAR_DIR}/systray-x@Ximi1970.xpi
      ;;
  esac
  
  #
  # Cleanup
  #
  rm -rf ./usr
  
  if [ ! -f systray-x@Ximi1970.xpi ] ; then
    cp -f ./${TAR_DIR}/systray-x@Ximi1970.xpi .
  fi

  #
  # Get JSON
  #
  cp -f ../app/config/linux/SysTray_X.json.template ./${TAR_DIR}/SysTray_X.json.template

  #
  # Create tar
  #
  tar -C ./${TAR_DIR} -cJf ${TAR_DIR}.tar.xz .

  #
  # Rename the RPM
  #
  if [ "${RPM_NAME_EXT}" != "_" ] ; then
    NEW_RPM_FILE=`echo ${RPM_FILE} | sed -s "s/\(systray-x-${FOUND_VERSION}-\)\(.*\)/\1${RPM_NAME_EXT}\.\2/"`
    mv -f ${RPM_FILE} $NEW_RPM_FILE
  fi

  #
  # Cleanup
  #
  rm -rf ${TAR_DIR}
}


create_deb_tar() {

  local REPO_BASE=$1
  local REPO_DISTR=$2
  local REPO_ARCH=$3
  local DEB_NAME_EXT=$4
  local GNOME_EXT=$5
  
  ##########################################
  #
  # Create the SysTray-X tar
  #
  ##########################################

  #
  # Get index.html
  #
  rm -f index.html
  wget -q "${REPO_BASE}/${REPO_DISTR}/${REPO_ARCH}/"
    
  #
  # Find deb
  #
  local DEB_FILE=$(grep ">systray-x_.*\.deb<" index.html | sed -e "s/.*>\(systray-x.*deb\)<.*/\1/")
  rm -f index.html

  echo "Found: "${DEB_FILE}
  
  FOUND_VERSION=$(echo ${DEB_FILE} | sed -e "s/systray-x_\(.*\)_.*/\1/")

  echo "Version: "${FOUND_VERSION}

  #
  # Create tar dir
  #
  local TAR_DIR=SysTray-X-${FOUND_VERSION}-${REPO_DISTR}

  rm -rf ${TAR_DIR}
  mkdir -p ${TAR_DIR}
    
  #
  # Add the gnome extension to the tar
  #
  if [ "${GNOME_EXT}" == "26" ] && [ -f gnome-shell-extension-26.tar.xz ] ; then
    cp -f gnome-shell-extension-26.tar.xz ${TAR_DIR}/gnome-shell-extension.tar.xz
  fi
  if [ "${GNOME_EXT}" == "33" ] && [ -f gnome-shell-extension-33.tar.xz ] ; then
    cp -f gnome-shell-extension-33.tar.xz ${TAR_DIR}/gnome-shell-extension.tar.xz
  fi
  
  #
  # Get the SysTray-X deb
  #
  wget -q "${REPO_BASE}/${REPO_DISTR}/${REPO_ARCH}/${DEB_FILE}"

  #
  # Extract 
  #
  dpkg --fsys-tarfile ${DEB_FILE} | tar xOf - ./usr/bin/SysTray-X > ./${TAR_DIR}/SysTray-X
  chmod 755 ./${TAR_DIR}/SysTray-X

  dpkg --fsys-tarfile ${DEB_FILE} | tar xOf - ./usr/lib/thunderbird-addons/extensions/systray-x@Ximi1970.xpi > ./${TAR_DIR}/systray-x@Ximi1970.xpi

  if [ ! -f systray-x@Ximi1970.xpi ] ; then
    cp -f ./${TAR_DIR}/systray-x@Ximi1970.xpi .
  fi

  #
  # Get JSON
  #
  cp -f ../app/config/linux/SysTray_X.json.template ./${TAR_DIR}/SysTray_X.json.template

  #
  # Create tar
  #
  tar -C ./${TAR_DIR} -cJf ${TAR_DIR}.tar.xz .

  #
  # Rename the DEB
  #
  if [ "${DEB_NAME_EXT}" != "_" ] ; then
    NEW_DEB_FILE=`echo ${DEB_FILE} | sed -s "s/\(systray-x\_${FOUND_VERSION}\_\)\(.*\)/\1${DEB_NAME_EXT}\_\2/"`
    mv -f ${DEB_FILE} ${NEW_DEB_FILE}
  fi

  #
  # Cleanup
  #
  rm -rf ${TAR_DIR}
}

#################################################################################
#
#
#   Main
#
#
#################################################################################

mkdir -p $TARGET_DIR
pushd $TARGET_DIR > /dev/null 2>&1

#
# Create bash installers for RPM based distributions
#
INDEX=1
for rpmdir in $OBS_RPM_ARCHS ; do

  echo
  echo
  echo "Generating installer: "${rpmdir}
  echo
  
  #
  # Get base of the repo
  #
  REPO_DISTR=$(echo ${rpmdir} | cut -d'/' -f1)
  REPO_ARCH=$(echo ${rpmdir} | cut -d'/' -f2)
  
  RPM_NAME_EXT=$(echo ${OBS_RPM_PKS} | cut -d' ' -f${INDEX})
  GNOME_EXT=$(echo ${OBS_RPM_GNOME_EXT} | cut -d' ' -f${INDEX})

  #
  # Generate the gnome tar file
  #
  create_gnome_extension_tar ${GNOME_APPINDICATOR} ${GNOME_EXT}
  
  #
  # Generate the SysTray-X tar file
  #
  create_rpm_tar ${OBS_REPO_BASE} ${REPO_DISTR} ${REPO_ARCH} ${RPM_NAME_EXT} ${GNOME_EXT}
  
  #
  # Create installer
  #
  INSTALLER=SysTray-X-${FOUND_VERSION}-${REPO_DISTR}-${REPO_ARCH}-install.sh
  cp -f ../dist/install.sh ${INSTALLER}

  #
  # Insert gnome setup
  #
  sed -i -e "/__GNOME_SETUP__/r../dist/install.${REPO_DISTR}.sh" ${INSTALLER}
  sed -i -e "s/__GNOME_SETUP__//" ${INSTALLER}

  #
  # Insert install tar
  #
  cat SysTray-X-${FOUND_VERSION}-${REPO_DISTR}.tar.xz >> ${INSTALLER}
  chmod 755 ${INSTALLER}

  #
  # Cleanup
  #
  rm -f SysTray-X-${FOUND_VERSION}-${REPO_DISTR}.tar.xz

  #
  # Update index
  #
  INDEX=$((INDEX+1))
done

#
# Cleanup
#
rm -f gnome-shell-extension.tar.xz

#
# Create bash installers for DEB based distributions
#
INDEX=1
for debdir in $OBS_DEB_ARCHS ; do
 
  echo
  echo
  echo "Generating installer: "${debdir}
  echo
  
  #
  # Get base of the repo
  #
  REPO_DISTR=$(echo ${debdir} | cut -d'/' -f1)
  REPO_ARCH=$(echo ${debdir} | cut -d'/' -f2)
  
  DEB_NAME_EXT=$(echo ${OBS_DEB_PKS} | cut -d' ' -f${INDEX})
  GNOME_EXT=$(echo ${OBS_DEB_GNOME_EXT} | cut -d' ' -f${INDEX})
  
  #
  # Generate the gnome tar file
  #
  create_gnome_extension_tar ${GNOME_APPINDICATOR} ${GNOME_EXT}

  #
  # Generate the SysTray-X tar file
  #
  create_deb_tar ${OBS_REPO_BASE} ${REPO_DISTR} ${REPO_ARCH} ${DEB_NAME_EXT} ${GNOME_EXT}
 
  #
  # Create installer
  #
  INSTALLER=SysTray-X-${FOUND_VERSION}-${REPO_DISTR}-${REPO_ARCH}-install.sh
  cp -f ../dist/install.sh ${INSTALLER}

  #
  # Insert gnome setup
  #
  sed -i -e "/__GNOME_SETUP__/r../dist/install.${REPO_DISTR}.sh" ${INSTALLER}
  sed -i -e "s/__GNOME_SETUP__//" ${INSTALLER}

  #
  # Insert install tar
  #
  cat SysTray-X-${FOUND_VERSION}-${REPO_DISTR}.tar.xz >> ${INSTALLER}
  chmod 755 ${INSTALLER}

  #
  # Cleanup
  #
  rm -f SysTray-X-${FOUND_VERSION}-${REPO_DISTR}.tar.xz

  #
  # Update index
  #
  INDEX=$((INDEX+1))
done 

#
# Cleanup
#
rm -f gnome-shell-extension-*.tar.xz

popd > /dev/null 2>&1
