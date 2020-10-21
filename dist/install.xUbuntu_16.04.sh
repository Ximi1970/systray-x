#
#   Check for Gnome
#
if [ "$XDG_CURRENT_DESKTOP" == "Unity" ] ; then
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
        dpkg -l | grep -q libqt5widgets5
        if [ "$?" == "1" ] ; then
            echo
            echo "Please install the package libqt5widgets5"
            echo
            echo "sudo apt install libqt5widgets5"
            echo
        fi
    fi
fi
