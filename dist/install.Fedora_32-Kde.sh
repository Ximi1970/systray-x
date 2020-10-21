##
#   Check for Kde
#
if [ "$XDG_CURRENT_DESKTOP" == "KDE" ] ; then
    #
    # Check for Qt
    #
    dnf list installed "qt5-qtbase*" | grep -q qt5-qtbase-gui
    if [ "$?" == "1" ] ; then
        echo
        echo "Please install the package qt5-qtbase-gui"
        echo
        echo "sudo dnf install qt5-qtbase-gui"
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
