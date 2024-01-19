Translations
============

Adding a languages
------------------

Add your language to the TRANSLATIONS var in the SysTray-X .pro file (app/SysTray-X/SysTray-X.pro)


Update translation files
------------------------

To update the base translation files in the languages directory:

cd app/SysTray-X
lupdate-qt5 -verbose SysTray-X.pro


Translating
-----------

Start Qt linguist (linguist-qt5, linguist) and open the language file (app/SysTray-X/languages/*.ts) you want to translate.
Translate the found strings, mark them translated. When finished, 'Save' the file and 'Release' the file.

Add the *.ts and *.qm files to git.


Qt translation tools
--------------------

lupdate -verbose SysTray-X.pro

linguist

lrelease SysTray-X.pro
