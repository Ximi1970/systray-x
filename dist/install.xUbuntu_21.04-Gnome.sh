enableGnomeExtension() {
    EXTENSION="ubuntu-appindicator@ubuntu.com"
    ENABLE_CMD="/usr/bin/gnome-extensions"
    ENABLE_CMD_OPTIONS="enable ${EXTENSION}"

    #
    #   Is the extension installed?
    #
    if [ -x $ENABLE_CMD ] ; then
        $ENABLE_CMD $ENABLE_CMD_OPTIONS
    fi
    
    echo
    echo "Please logout and login to activate the gnome shell extension"
    echo
}

#
#   Check for Gnome
#
if [ "$XDG_CURRENT_DESKTOP" == "ubuntu:GNOME" ] ; then
    enableGnomeExtension

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
else
    #
    #   Check for KDE
    #
    if [ "$XDG_CURRENT_DESKTOP" == "KDE" ] ; then
        echo
        echo "Please use the __KDE_INSTALLER__ installer."
        echo "This setup will not work."
        echo
    else
        echo
        echo "Please use the __MINIMAL_INSTALLER__ installer."
        echo "This setup will not work."
        echo
    fi
fi
