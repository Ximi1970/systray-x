##
#   Check for Kde
#
if [ "$XDG_CURRENT_DESKTOP" == "KDE" ] ; then
    #
    # Check for Qt
    #
    dnf list installed "libQt5*" | grep -q libQt5Widgets5
    if [ "$?" == "1" ] ; then
        echo
        echo "Please install the package libQt5Widgets5"
        echo
        echo "sudo dnf install libQt5Widgets5"
        echo
    fi

    dnf list installed "kf5*" | grep -q kf5-knotifications
    if [ "$?" == "1" ] ; then
        echo
        echo "Please install the package kf5-knotifications"
        echo
        echo "sudo dnf install kf5-knotifications"
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
