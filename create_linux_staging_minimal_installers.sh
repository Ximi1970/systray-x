#!/bin/bash

VERSION=`git describe --long | sed "s/-.*//"`

OBS_REPO_BASE="https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons:/Staging"
TARGET_DIR="bin-staging"

OBS_PACKAGE="systray-x-minimal"

ENABLE_RPM=true
ENABLE_DEB=true
ENABLE_PAC=true

OBS_RPM_ARCHS=""
OBS_RPM_PKS=""
OBS_RPM_ARCHS+="openSUSE_Leap_15.4/x86_64 "
OBS_RPM_PKS+="_ "
OBS_RPM_ARCHS+="openSUSE_Tumbleweed/i586 "
OBS_RPM_PKS+="tmblwd "
OBS_RPM_ARCHS+="openSUSE_Tumbleweed/x86_64 "
OBS_RPM_PKS+="tmblwd "
OBS_RPM_ARCHS+="15.4/x86_64 "
OBS_RPM_PKS+="lp154 "
OBS_RPM_ARCHS+="15.5/x86_64 "
OBS_RPM_PKS+="lp155 "
OBS_RPM_ARCHS+="Fedora_37/x86_64 "
OBS_RPM_PKS+="fed37 "
OBS_RPM_ARCHS+="Fedora_38/x86_64 "
OBS_RPM_PKS+="fed38 "
OBS_RPM_ARCHS+="Fedora_39/x86_64 "
OBS_RPM_PKS+="fed39 "
OBS_RPM_ARCHS+="Fedora_Rawhide/x86_64 "
OBS_RPM_PKS+="fedraw "


OBS_DEB_ARCHS=""
OBS_DEB_PKS=""
OBS_DEB_ARCHS+="Debian_10/i386 "
OBS_DEB_PKS+="deb10 "
OBS_DEB_ARCHS+="Debian_10/amd64 "
OBS_DEB_PKS+="deb10 "
OBS_DEB_ARCHS+="Debian_11/i386 "
OBS_DEB_PKS+="deb11 "
OBS_DEB_ARCHS+="Debian_11/amd64 "
OBS_DEB_PKS+="deb11 "
OBS_DEB_ARCHS+="Debian_12/i386 "
OBS_DEB_PKS+="deb12 "
OBS_DEB_ARCHS+="Debian_12/amd64 "
OBS_DEB_PKS+="deb12 "
OBS_DEB_ARCHS+="Debian_Unstable/amd64 "
OBS_DEB_PKS+="debunstable "
OBS_DEB_ARCHS+="Debian_Testing/amd64 "
OBS_DEB_PKS+="debtest "
OBS_DEB_ARCHS+="xUbuntu_18.04/i386 "
OBS_DEB_PKS+="bionic1804 "
OBS_DEB_ARCHS+="xUbuntu_18.04/amd64 "
OBS_DEB_PKS+="bionic1804 "
OBS_DEB_ARCHS+="xUbuntu_20.04/amd64 "
OBS_DEB_PKS+="focal2004 "
OBS_DEB_ARCHS+="xUbuntu_22.04/amd64 "
OBS_DEB_PKS+="jammy2022 "
OBS_DEB_ARCHS+="xUbuntu_23.04/amd64 "
OBS_DEB_PKS+="lunar2023 "
OBS_DEB_ARCHS+="xUbuntu_23.10/amd64 "
OBS_DEB_PKS+="mantic2023 "


OBS_PAC_ARCHS=""
OBS_PAC_PKS=""
OBS_PAC_ARCHS+="Arch/x86_64 "
OBS_PAC_PKS+="arch "


create_rpm_tar() {

  local REPO_BASE=$1
  local REPO_DISTR=$2
  local REPO_ARCH=$3
  local RPM_NAME_EXT=$4

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
  local RPM_FILE=$(grep ">${OBS_PACKAGE}-.*${VERSION}.*rpm<" index.html | sed -e "s/.*>\(${OBS_PACKAGE}-.*rpm\)<.*/\1/")

  rm -f index.html

  echo "Found: "${RPM_FILE}
  
  FOUND_VERSION=$(echo ${RPM_FILE} | sed -e "s/${OBS_PACKAGE}-\(.*\)-.*/\1/")

  echo "Version: "${FOUND_VERSION}

  #
  # Create tar dir
  #
  local TAR_DIR=SysTray-X-${FOUND_VERSION}-${REPO_DISTR}

  rm -rf ${TAR_DIR}
  mkdir -p ${TAR_DIR}
  
  #
  # Get the SysTray-X rpm
  #
  if [ ! -f ${RPM_FILE} ] ; then
    wget -q "${REPO_BASE}/${REPO_DISTR}/${REPO_ARCH}/${RPM_FILE}"
  fi
  
  #
  # Get compression type
  #
  COMPRESSION=$(rpm -qp --qf '%{PAYLOADCOMPRESSOR}\n' ${RPM_FILE})
#  echo ${COMPRESSION}
  
  case ${COMPRESSION} in
#    zstd)
      #
      # Extract 
      #
#      rpm2cpio ${RPM_FILE} | zstd -d | cpio --quiet -idm

      #
      # Get files
      #
#      cp -f ./usr/bin/SysTray-X ./${TAR_DIR}/SysTray-X
#      chmod 755 ./${TAR_DIR}/SysTray-X
#      cp -f ./usr/share/doc/${OBS_PACKAGE}/systray-x@Ximi1970.xpi ./${TAR_DIR}/systray-x@Ximi1970.xpi
#      ;;
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
      if [ -d ./usr/share/doc/packages ] ; then
        cp -f ./usr/share/doc/packages/${OBS_PACKAGE}/systray-x@Ximi1970.xpi ./${TAR_DIR}/systray-x@Ximi1970.xpi
      else
        if [ -d ./usr/share/doc/${OBS_PACKAGE} ] ; then
          cp -f ./usr/share/doc/${OBS_PACKAGE}/systray-x@Ximi1970.xpi ./${TAR_DIR}/systray-x@Ximi1970.xpi
        else
          cp -f ./usr/share/doc/${OBS_PACKAGE}*/systray-x@Ximi1970.xpi ./${TAR_DIR}/systray-x@Ximi1970.xpi
        fi
      fi
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
    NEW_RPM_FILE=`echo ${RPM_FILE} | sed -s "s/\(${OBS_PACKAGE}-${FOUND_VERSION}-\)\(.*\)/\1${RPM_NAME_EXT}\.\2/"`
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
  local DEB_FILE=$(grep ">${OBS_PACKAGE}_${VERSION}.*\.deb<" index.html | sed -e "s/.*>\(${OBS_PACKAGE}.*deb\)<.*/\1/")
  rm -f index.html

  echo "Found: "${DEB_FILE}
  
  FOUND_VERSION=$(echo ${DEB_FILE} | sed -e "s/${OBS_PACKAGE}_\(.*\)_.*/\1/")

  echo "Version: "${FOUND_VERSION}

  #
  # Create tar dir
  #
  local TAR_DIR=SysTray-X-${FOUND_VERSION}-${REPO_DISTR}

  rm -rf ${TAR_DIR}
  mkdir -p ${TAR_DIR}
  
  #
  # Get the SysTray-X deb
  #
  if [ ! -f ${DEB_FILE} ] ; then
    wget -q "${REPO_BASE}/${REPO_DISTR}/${REPO_ARCH}/${DEB_FILE}"
  fi
  
  #
  # Extract 
  #
  dpkg-deb --fsys-tarfile ${DEB_FILE} | tar xOf - ./usr/bin/SysTray-X > ./${TAR_DIR}/SysTray-X
  chmod 755 ./${TAR_DIR}/SysTray-X

  dpkg-deb --fsys-tarfile ${DEB_FILE} | tar xOf - ./usr/lib/thunderbird-addons/extensions/systray-x@Ximi1970.xpi > ./${TAR_DIR}/systray-x@Ximi1970.xpi

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
    NEW_DEB_FILE=`echo ${DEB_FILE} | sed -s "s/\(${OBS_PACKAGE}\_${FOUND_VERSION}\_\)\(.*\)/\1${DEB_NAME_EXT}\_\2/"`
    mv -f ${DEB_FILE} ${NEW_DEB_FILE}
  fi

  #
  # Cleanup
  #
  rm -rf ${TAR_DIR}
}


create_pac_tar() {

  local REPO_BASE=$1
  local REPO_DISTR=$2
  local REPO_ARCH=$3
  local PAC_NAME_EXT=$4

  ##########################################
  #
  # Create the SysTray-X tar
  #
  ##########################################

  #
  # Get index.html
  #
  rm -f index.html
  curl -s "${REPO_BASE}/${REPO_DISTR}/${REPO_ARCH}/" > index.html
  
  #
  # Find rpm
  #
  local PAC_FILE=$(grep ">${OBS_PACKAGE}-[^d].*${VERSION}.*\.zst<" index.html | sed -e "s/.*>\(${OBS_PACKAGE}-[^d].*zst\)<.*/\1/")
  rm -f index.html

  echo "Found: "${PAC_FILE}
  
  FOUND_VERSION=$(echo ${PAC_FILE} | sed -e "s/${OBS_PACKAGE}-\(.*\)-.*/\1/")

  echo "Version: "${FOUND_VERSION}

  #
  # Create tar dir
  #
  local TAR_DIR=SysTray-X-${FOUND_VERSION}-${REPO_DISTR}

  rm -rf ${TAR_DIR}
  mkdir -p ${TAR_DIR}
  
  #
  # Get the SysTray-X rpm
  #
  if [ ! -f ${PAC_FILE} ] ; then
    wget -q "${REPO_BASE}/${REPO_DISTR}/${REPO_ARCH}/${PAC_FILE}"
  fi

  if [ ! -f systray-x.tar.zst ] ; then
    cp -f ${PAC_FILE} systray-x.tar.zst
  fi

  tar -I zstd -xf systray-x.tar.zst
  
  rm -f systray-x.tar.zst
  rm -f .BUILDINFO
  rm -f .INSTALL
  rm -f .MTREE
  rm -f .PKGINFO

  #
  # Get files
  #
  cp -f ./usr/bin/SysTray-X ./${TAR_DIR}/SysTray-X
  chmod 755 ./${TAR_DIR}/SysTray-X
  cp -f ./usr/lib/thunderbird/distribution/extensions/systray-x@Ximi1970.xpi ./${TAR_DIR}/systray-x@Ximi1970.xpi
  
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
#  tar -C "./${TAR_DIR}" -cJf "${TAR_DIR}.tar.xz" .
  tar -C "./${TAR_DIR}" -cJf systray-x.tar.xz .
  mv -f systray-x.tar.xz ${TAR_DIR}.tar.xz

  #
  # Rename the PAC
  #
  if [ "${PAC_NAME_EXT}" != "_" ] ; then
    NEW_PAC_FILE=`echo ${PAC_FILE} | sed -s "s/\(${OBS_PACKAGE}-${FOUND_VERSION}-\)\(.*\)/\1${PAC_NAME_EXT}\.\2/"`
    mv -f "${PAC_FILE}" "$NEW_PAC_FILE"
  fi

  #
  # Cleanup
  #
  rm -rf "${TAR_DIR}"
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

if [ "$ENABLE_RPM" = true ] ; then
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
    
    #
    # Generate the SysTray-X tar file
    #
    create_rpm_tar ${OBS_REPO_BASE} ${REPO_DISTR} ${REPO_ARCH} ${RPM_NAME_EXT}
    
    #
    # Create installer
    #
    INSTALLER=SysTray-X-${FOUND_VERSION}-${REPO_DISTR}-${REPO_ARCH}-Minimal-install.sh
    cp -f ../dist/install.sh ${INSTALLER}

    #
    # Insert Minimal setup
    #
    sed -i -e "/__XXXX_SETUP__/r../dist/install.${REPO_DISTR}.sh" ${INSTALLER}
    sed -i -e "s/__GNOME_INSTALLER__/SysTray-X-${FOUND_VERSION}-${REPO_DISTR}-${REPO_ARCH}-GNOME-install.sh/" ${INSTALLER}
    sed -i -e "s/__KDE_INSTALLER__/SysTray-X-${FOUND_VERSION}-${REPO_DISTR}-${REPO_ARCH}-KDE-install.sh/" ${INSTALLER}
    sed -i -e "s/__XXXX_SETUP__//" ${INSTALLER}

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
fi

if [ "$ENABLE_DEB" = true ] ; then
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

    #
    # Generate the SysTray-X tar file
    #
    create_deb_tar ${OBS_REPO_BASE} ${REPO_DISTR} ${REPO_ARCH} ${DEB_NAME_EXT}
    
    #
    # Create installer
    #
    INSTALLER=SysTray-X-${FOUND_VERSION}-${REPO_DISTR}-${REPO_ARCH}-Minimal-install.sh
    cp -f ../dist/install.sh ${INSTALLER}

    #
    # Insert Minimal setup
    #
    sed -i -e "/__XXXX_SETUP__/r../dist/install.${REPO_DISTR}.sh" ${INSTALLER}
    sed -i -e "s/__GNOME_INSTALLER__/SysTray-X-${FOUND_VERSION}-${REPO_DISTR}-${REPO_ARCH}-GNOME-install.sh/" ${INSTALLER}
    sed -i -e "s/__KDE_INSTALLER__/SysTray-X-${FOUND_VERSION}-${REPO_DISTR}-${REPO_ARCH}-KDE-install.sh/" ${INSTALLER}
    sed -i -e "s/__XXXX_SETUP__//" ${INSTALLER}

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
fi

if [ "$ENABLE_PAC" = true ] ; then
    #
    # Create bash installers for PAC based distributions
    #
    INDEX=1
    for pacdir in $OBS_PAC_ARCHS ; do
    
    echo
    echo
    echo "Generating installer: "${pacdir}
    echo
    
    #
    # Get base of the repo
    #
    REPO_DISTR=$(echo ${pacdir} | cut -d'/' -f1)
    REPO_ARCH=$(echo ${pacdir} | cut -d'/' -f2)
    
    PAC_NAME_EXT=$(echo ${OBS_PAC_PKS} | cut -d' ' -f${INDEX})

    #
    # Generate the SysTray-X tar file
    #
    create_pac_tar ${OBS_REPO_BASE} ${REPO_DISTR} ${REPO_ARCH} ${PAC_NAME_EXT}
    
    #
    # Create installer
    #
    INSTALLER=SysTray-X-${FOUND_VERSION}-${REPO_DISTR}-${REPO_ARCH}-Minimal-install.sh
    cp -f ../dist/install.sh ${INSTALLER}

    #
    # Insert Minimal setup
    #
    sed -i -e "/__XXXX_SETUP__/r../dist/install.${REPO_DISTR}.sh" ${INSTALLER}
    sed -i -e "s/__GNOME_INSTALLER__/SysTray-X-${FOUND_VERSION}-${REPO_DISTR}-${REPO_ARCH}-GNOME-install.sh/" ${INSTALLER}
    sed -i -e "s/__KDE_INSTALLER__/SysTray-X-${FOUND_VERSION}-${REPO_DISTR}-${REPO_ARCH}-KDE-install.sh/" ${INSTALLER}
    sed -i -e "s/__XXXX_SETUP__//" ${INSTALLER}

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
fi

popd > /dev/null 2>&1
