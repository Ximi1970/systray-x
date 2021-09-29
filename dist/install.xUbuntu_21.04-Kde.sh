#
#   Check for KDE
#
if [ "$XDG_CURRENT_DESKTOP" == "KDE" ] ; then
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
    
    dpkg -l | grep -q libkf5notifications5
    if [ "$?" == "1" ] ; then
        echo
        echo "Please install the package libkf5notifications5"
        echo
        echo "sudo apt install libkf5notifications5"
        echo
    fi
else
    #
    #   Check for Gnome
    #
    if [ "$XDG_CURRENT_DESKTOP" == "ubuntu:GNOME" ] ; then
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
