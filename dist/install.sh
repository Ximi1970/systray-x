#!/bin/bash

INSTALL_HOME=`echo ~`

if [ "$1" == "-h" ] ; then
  echo
  echo "Usage: $0 [abs-install-path [profiles-path] ]"
  echo
  echo "  Default install path: $INSTALL_HOME/.mozilla/native-messaging-hosts"
  echo "  Default profiles: $INSTALL_HOME/.thunderbird"
  echo
  exit 0
fi

if [ -n "$1" ] ; then
  DESTINATION="$1"

  if [ -n "$2" ] ; then
    PROFILES="$2"
  fi
else
  DESTINATION="$INSTALL_HOME/.mozilla/native-messaging-hosts"
  PROFILES="$INSTALL_HOME/.thunderbird"
fi

#
# Tell the user what we are going to do
#
echo
echo "Installing SysTray-X companion app to:"
echo "$DESTINATION"
echo
echo "Installing SysTray-X addon into all profiles listed in:"
echo "$PROFILES/profiles.ini"
echo
read -n 1 -s -r -p "Press any key to continue, Ctrl-C to cancel"
echo

#
# Install
#
echo "Installing SysTray-X companion app..."

#
# Create destination
#
mkdir -p ${DESTINATION}

#
# Find __ARCHIVE__ maker, read archive content and decompress it
#
ARCHIVE=$(awk '/^__ARCHIVE__/ {print NR + 1; exit 0; }' "${0}")
tail -n+${ARCHIVE} "${0}" | tar xpJ -C ${DESTINATION}

#
# Set the config
#
INSTALL_HOME=`echo ~`
sed -i "s?SYSTRAY_X_PATH?${DESTINATION}/SysTray-X?" ${DESTINATION}/SysTray_X.json.template
mv -f ${DESTINATION}/SysTray_X.json.template ${DESTINATION}/SysTray_X.json

#
# Installing the add-on
#
echo "Installing SysTray-X addon..."

for i in `grep "Path=" $PROFILES/profiles.ini | sed -e "s/Path=//"` ; do
  mkdir -p $PROFILES/$i/extensions
  cp -f ${DESTINATION}/systray-x@Ximi1970.xpi $PROFILES/$i/extensions
done

#
# Done
#
echo ""
echo "Addon and companion app installation complete."
echo "App en JSON config have been installed in: $DESTINATION"
echo ""
echo "Do not forget to install the Thunderbird SysTray-X Add-on"
echo "using the standard Thunderbird Add-ons / Extensions store !"
echo ""
exit 0

__ARCHIVE__
