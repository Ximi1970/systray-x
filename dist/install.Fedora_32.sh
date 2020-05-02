enableGnomeExtension() {
    EXTENSION="appindicatorsupport@rgcjonas.gmail.com"
    PACKAGE="gnome-shell-extension-appindicator"
    ENABLE_CMD="gnome-extensions enable ${EXTENSION}"
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
#   Enable the gnome shell extension for the local user
#
if [ "$XDG_CURRENT_DESKTOP" == "GNOME" ] ; then
    enableGnomeExtension
fi
