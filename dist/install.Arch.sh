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
        pacman -Q qt5-base 2>/dev/null | grep -q qt5-base
        if [ "$?" == "1" ] ; then
            echo
            echo "Please install the package qt5-base"
            echo
            echo "sudo pacman -S qt5-base"
            echo
        fi
    fi
fi
