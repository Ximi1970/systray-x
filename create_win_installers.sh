#!/bin/bash
UPLOAD_SERVER="upload.server.com"
UPLOAD_USER="user"
UPLOAD_PASSWORD="password"
UPLOAD_DIR="~/src/systray-x/bin"

BUILD_FARM="build.farm.com"
BUILD_FARM_USER="farmuser"

BUILD_WORKER="worker.build.farm.com"
BUILD_WORKER_USER="BuildUser"
BUILD_WORKER_VMNAME="Virtual Machine Name"

VERSION=`git describe --long | sed "s/-.*//"`

#
# Start the worker on the farm
#
ssh ${BUILD_FARM_USER}@${BUILD_FARM} <<-ENDSSH
  VBoxHeadless -startvm "${BUILD_WORKER_VMNAME}" > /dev/null 2>&1 &
ENDSSH

#
# Wait for the worker to become active
#
timeout=30
t=0
while [ $t -lt $timeout ] ; do
  ping -c1 ${BUILD_WORKER}

  if [ $? -eq 0 ] ; then
    break;
  fi

  sleep 10
  
  t=$((t+1))
done

if [ $t -eq $timeout ] ; then
  echo "Cannot find server"
  exit 1
fi

#
# Start build
#
ssh ${BUILD_WORKER_USER}@${BUILD_WORKER} -T <<-ENDSSH
  cd /cygdrive/c/Users/${BUILD_WORKER_USER}/Documents
  mkdir -p Projects/Github/Ximi1970
  cd Projects/Github/Ximi1970
  rm -rf systray-x
  git clone https://github.com/Ximi1970/systray-x.git
  cd systray-x
  make
  cd dist
  /cygdrive/c/Program\ Files\ \(x86\)/NSIS/Bin/makensis.exe SysTray-X-32bit.nsi
  /cygdrive/c/Program\ Files\ \(x86\)/NSIS/Bin/makensis.exe SysTray-X-64bit.nsi
  mv -f SysTray-X-setup32.exe SysTray-X-setup32-${VERSION}.exe
  mv -f SysTray-X-setup64.exe SysTray-X-setup64-${VERSION}.exe  
  lftp -u ${UPLOAD_USER},${UPLOAD_PASSWORD} ${UPLOAD_SERVER}/${UPLOAD_DIR} <<-ENDFTP
    mput SysTray-X-setup*.exe
    close
    quit
ENDFTP
  exit
ENDSSH

#
# Shutdown worker
#
ssh ${BUILD_FARM_USER}@${BUILD_FARM} <<-ENDSSH
  VBoxManage controlvm "${BUILD_WORKER_VMNAME}" acpipowerbutton
ENDSSH
