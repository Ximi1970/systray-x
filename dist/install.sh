#!/bin/bash

INSTALL_HOME=`echo ~`

if [ "$1" == "-h" ] ; then
  echo
  echo "Usage: $0 [abs-install-path]"
  echo
  echo "  Default install path: $INSTALL_HOME/.mozilla/native-messaging-hosts"
  echo
  exit 0
fi

if [ -n "$1" ] ; then
  DESTINATION="$1"
else
  DESTINATION="$INSTALL_HOME/.mozilla/native-messaging-hosts"
fi

#
# Tell the user what we are going to do
#
echo
echo "Installing SysTray-X companion app to:"
echo "$DESTINATION"
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
# Done
#
echo ""
echo "Companion app installation complete."
echo "App en JSON config have been installed in: $DESTINATION"
echo ""
echo "Do not forget to install the Thunderbird SysTray-X Add-on"
echo "using the standard Thunderbird Add-ons / Extensions store !"
echo ""
exit 0

__ARCHIVE__
