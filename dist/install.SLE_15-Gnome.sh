enableGnomeExtension() {
    EXTENSION="appindicatorsupport@rgcjonas.gmail.com"
    PACKAGE="gnome-shell-extension-appindicator"
    ENABLE_CMD="gnome-shell-extension-tool -e ${EXTENSION}"
   
    if [ ! -x /usr/bin/gnome-shell-extension-tool ] ; then
      echo
      echo "Please install the package gnome-shell-devel:"
      echo
      echo "sudo zypper install gnome-shell-devel"
      echo
      echo "And run after the installer is finished:"
      echo
      echo $ENABLE_CMD
      echo
    fi

    #
    #   Is the extension installed?
    #
    if [ -d /usr/share/gnome-shell/extensions/${EXTENSION} ] || [ -d ~/.local/share/gnome-shell/extensions/${EXTENSION} ] ; then
        $ENABLE_CMD
    else
        mkdir -p ~/.local/share/gnome-shell/extensions
        tar -C ~/.local/share/gnome-shell/extensions -xJf ${DESTINATION}/gnome-shell-extension.tar.xz
        $ENABLE_CMD
    fi
    
    echo
    echo "Please logout and login to activate the gnome shell extension"
    echo
}

#
#   Check for Gnome
#
if [ "$XDG_CURRENT_DESKTOP" == "GNOME" ] ; then
    enableGnomeExtension

    #
    # Check for Qt
    #
    rpm -qa | grep -q libQt5Widgets5
    if [ "$?" == "1" ] ; then
        echo
        echo "Please install the package libqt5widgets5"
        echo
        echo "sudo zypper install libQt5Widgets5"
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
