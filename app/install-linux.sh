#!/bin/bash

if [ -z $1 ] ; then
  echo "Usage: $0 <tb-config-path>"
  exit 1
fi

mkdir .mozilla/native-messaging-hosts
#mkdir .mozilla/managed-storage
#mkdir .mozilla/pkcs11-modules

cp ping_pong.json .mozilla/native-messaging-hosts/ping_pong.json
#cp ping_pong.json .mozilla/managed-storage/ping_pong.json
#cp ping_pong.json .mozilla/pkcs11-modules/ping_pong.json 
