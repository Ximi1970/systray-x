#
#   Check for Gnome
#
if [ "$XDG_CURRENT_DESKTOP" == "GNOME" ] ; then
      echo
      echo "Please use the __GNOME_INSTALLER__ installer."
      echo "This setup will not work."
      echo
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
    fi
fi
