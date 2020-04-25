enableGnomeExtension() {
    EXTENSION="ubuntu-appindicator@ubuntu.com"
    ENABLE_CMD="gnome-extensions enable ${EXTENSION}"
    #
    #   Is the extension installed?
    #
    $ENABLE_CMD
    
    echo
    echo "Please logout and login to activate the gnome shell extension"
    echo
}

#
#   Enable the gnome shell extension for the local user
#
if [ "$XDG_CURRENT_DESKTOP" == "ubuntu:GNOME" ] ; then
    enableGnomeExtension
fi

#
# Check for Qt
#
dpkg -l | grep -q libqt5widgets5
if [ "$?" == "1" ] ; then
  echo
  echo "Please install the package libqt5widgets5"
  echo
  echo "sudo apt install libqt5widgets5"
  echo
fi
