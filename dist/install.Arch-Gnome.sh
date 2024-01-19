enableGnomeExtension() {
    EXTENSION="appindicatorsupport@rgcjonas.gmail.com"
    PACKAGE="gnome-shell-extension-appindicator"
    ENABLE_CMD="/usr/bin/gnome-shell-extension-tool"
    ENABLE_CMD_OPTIONS="-e ${EXTENSION}"

    #
    #   Is the extension installed?
    #
    if [ -d /usr/share/gnome-shell/extensions/${EXTENSION} ] || [ -d ~/.local/share/gnome-shell/extensions/${EXTENSION} ] ; then
        if [ -x $ENABLE_CMD ] ; then
            $ENABLE_CMD $ENABLE_CMD_OPTIONS
        fi
    else
        mkdir -p ~/.local/share/gnome-shell/extensions
        tar -C ~/.local/share/gnome-shell/extensions -xJf ${DESTINATION}/gnome-shell-extension.tar.xz
        if [ -x $ENABLE_CMD ] ; then
            $ENABLE_CMD $ENABLE_CMD_OPTIONS
        fi
    fi
    
    echo
    echo "Please logout and login to activate the gnome shell extension"
    echo
}

#
#   Enable the gnome shell extension for the local user
#
if [ "$XDG_CURRENT_DESKTOP" == "GNOME" ] ; then
    enableGnomeExtension

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
