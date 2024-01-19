function handleKeydown( e ) {
    self = e.target;

    if( e.shiftKey && e.key !== "Shift" )
    {
        const upperKey = e.key.toUpperCase();
        self.value = "Shift+" + upperKey;
        e.preventDefault();

//        console.debug("Shiftkey pressed with key " + upperKey)
    }

    if( e.altKey && e.key !== "Alt" )
    {
        const upperKey = e.key.toUpperCase();
        self.value = "Alt+" + upperKey;
        e.preventDefault();

//        console.debug("Altkey pressed with key " + upperKey)
    }

    if( e.ctrlKey && e.key !== "Control" )
    {
        const upperKey = e.key.toUpperCase();
        self.value = "Ctrl+" + upperKey;
        e.preventDefault();

//        console.debug("Ctrlkey pressed with key " + upperKey)
    }

    if( e.metaKey && e.key !== "Meta" )
    {
        const upperKey = e.key.toUpperCase();
        self.value = "Meta+" + upperKey;
        e.preventDefault();

//        console.debug("MetaKey pressed with key " + upperKey)
    }

    if( !e.shiftKey && !e.altKey && !e.ctrlKey && !e.metaKey )
    {
//        console.debug( "Key " + e.key );

        const specialJsKeys = [
            "Enter",
            "Backspace",
            "Delete",
            "Escape",
            "Insert",
            " ",
            "NumLock",
            "PageUp",
            "PageDown",
            "ArrowUp",
            "ArrowDown",
            "ArrowLeft",
            "ArrowRight",
            "Home",
            "End",
            "CapsLock",
            "OS",
            "Tab",
        ];

        const specialQtKeys = [
            "Return",
            "Backspace",
            "Del",
            "Esc",
            "Ins",
            "Space",
            "NumLock",
            "PgUp",
            "PgDown",
            "Up",
            "Down",
            "Left",
            "Right",
            "Home",
            "End",
            "CapsLock",
            "Meta",
            "",
        ];

        const specialIndex = specialJsKeys.indexOf( e.key );
        if( specialIndex !== -1 ) {
            self.value = specialQtKeys[ specialIndex ];
        } else {
            const upperKey = e.key.toUpperCase();
            self.value = upperKey;
        }
        e.preventDefault();    

//        console.debug( "Key " + upperKey );
    }
};

const element = document.getElementById("showHideShortcutInput");
element.onkeydown = handleKeydown;

function showHideShortcutClear() {
    const element = document.getElementById("showHideShortcutInput");
    element.value = "";

//    console.debug("Clear shortcut");
}

document
    .querySelector('[name="showhideshortcutbutton"]')
    .addEventListener("click", showHideShortcutClear);
