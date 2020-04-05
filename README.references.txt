#<script>


Installers

Bash:

https://www.matteomattei.com/create-self-contained-installer-in-bash-that-extracts-archives-and-perform-actitions/

Windows:

The good old Nullsoft:
https://sourceforge.net/projects/nsis/



Assets uploader:

https://github.com/tcnksm/ghr



Windows
=========================================

c# remove 3rd party application from taskbar
(first answer)

https://stackoverflow.com/questions/10514882/c-sharp-remove-3rd-party-application-from-taskbar

Close window
https://docs.microsoft.com/en-us/windows/win32/learnwin32/closing-the-window

Block close button
https://devblogs.microsoft.com/oldnewthing/20100604-00/?p=13803



Get processes
(EnumProcesses function)
https://www.qtcentre.org/threads/46145-Get-All-Running-Process-Win32


Get (Pid) and PPid
https://stackoverflow.com/questions/185254/how-can-a-win32-process-get-the-pid-of-its-parent
https://stackoverflow.com/questions/18401572/c-how-to-fetch-parent-process-id

https://www.codeproject.com/Articles/9893/Get-Parent-Process-PID



X11
==========================================

Get window ID from pid

https://unix.stackexchange.com/questions/321079/how-to-get-window-id-from-process-id

xdotool search --pid [pid]


https://stackoverflow.com/questions/20580897/get-x-window-id-from-process-in-bash

wmctrl -lp

xprop




Get processes (parent pid, pid and arguments)

ps -eo ppid,pid,args


qint64 QCoreApplication::applicationPid() to get systrayx pid -> parent pid -> tb window.


ps -p [PID] -o ppid


Bash 

xwininfo -tree -root



How to get the list of open windows from xserver

https://stackoverflow.com/questions/252906/how-to-get-the-list-of-open-windows-from-xserver
  -> XQueryTree Xlib
  
https://tronche.com/gui/x/xlib/window-information/XQueryTree.html
  

https://stackoverflow.com/questions/11594184/getting-a-list-of-window-wids-in-qt


How to detect window state in linux?

https://stackoverflow.com/questions/37003959/how-to-detect-window-state-in-linux



Taskbar hide

https://stackoverflow.com/questions/41622735/hide-window-from-linux-taskbar


minimize fullscreen Xlib OpenGL Window

https://stackoverflow.com/questions/6381098/minimize-fullscreen-xlib-opengl-window


Close windows

https://stackoverflow.com/questions/41202622/xlib-soft-closing-a-window



Intercept close event X11

https://stackoverflow.com/questions/1157364/intercept-wm-delete-window-on-x11


Allowed actions

https://stackoverflow.com/questions/28495586/x11-how-to-set-net-wm-allowed-actions


Qt X11

https://doc.qt.io/qt-5/qx11info.html


Obesolete X11 container qt4

https://stackoverflow.com/questions/39515384/qx11embedcontainer-alternative-in-qt5


Example container

https://stackoverflow.com/questions/1102658/qx11embedwidget-and-qx11embedcontainer


==========================================


Arguments for <script>
https://stackoverflow.com/questions/5292372/how-to-pass-parameters-to-a-script-tag


Loading css using <script>
https://stackoverflow.com/questions/574944/how-to-load-up-css-files-using-javascript


Input <input>
https://www.w3schools.com/tags/tag_input.asp

Input types
https://www.w3schools.com/tags/att_input_type.asp


Tree view
https://www.w3schools.com/howto/howto_js_treeview.asp

Nested table
https://codepen.io/st-iv/pen/xxbRxEj



Tree table
https://stackoverflow.com/questions/5636375/how-to-create-a-collapsing-tree-table-in-html-css-js
http://maxdesign.com.au/articles/tree-table/


Image to Base64 conversion

https://stackoverflow.com/questions/6150289/how-to-convert-image-into-base64-string-using-javascript
