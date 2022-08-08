#
#   Check for Kde
#
if [ "$XDG_CURRENT_DESKTOP" == "KDE" ] ; then
    #
    # Check for Qt
    #
    rpm -qa | grep -q libQt5Widgets5
    if [ "$?" == "1" ] ; then
        echo
        echo "Please install the package libQt5Widgets5"
        echo
        echo "sudo zypper install libQt5Widgets5"
        echo
    fi

    rpm -qa | grep -q libKF5Notifications5
    if [ "$?" == "1" ] ; then
        echo
        echo "Please install the package libKF5Notifications5"
        echo
        echo "sudo zypper install libKF5Notifications5"
        echo
    fi
else
    #
    #   Check for Gnome
    #
    if [ "$XDG_CURRENT_DESKTOP" == "GNOME" ] ; then
        echo
        echo "Please use the __GNOME_INSTALLER__ installer."
        echo "This setup will not work."
        echo
    else
        echo
        echo "Please use the __MINIMAL_INSTALLER__ installer."
        echo "This setup will not work."
        echo
    fi
fi
