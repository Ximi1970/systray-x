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
    dnf list installed "qt5-qtbase*" | grep -q qt5-qtbase-gui
        if [ "$?" == "1" ] ; then
            echo
            echo "Please install the package qt5-qtbase-gui"
            echo
            echo "sudo dnf install qt5-qtbase-gui"
            echo
        fi
    fi
fi
