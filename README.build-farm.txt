Install VirtualBox
==================

  zypper install virtualbox freerdp

  
Install your guest systems.
Use bridged networking so your VMs are visible on your local net.
Enable remote desktop for your guests for easy access later.




Starting a Virtual Machine
==========================

Find your VM with:

  VBoxManage list vms

Start the VM headless:

  VBoxHeadless -s "Windows 10"
  

Start a full screen remote session:

xfreerdp /u:<user> /v:<vms-ip> /f




  

Windows 10
==========


Packages to install:

* Visual Studio Express 2017
    (https://visualstudio.microsoft.com/vs/express/, https://aka.ms/vs/15/release/vs_WDExpress.exe
* Visual Studio Express 2019
    (https://visualstudio.microsoft.com/vs/express/)
* TortoiseGit (https://tortoisegit.org/)
* Git for Windows (https://gitforwindows.org/)
* PuTTY (https://www.putty.org/)

* Qt (https://www.qt.io/)
* Jom (https://wiki.qt.io/Jom)

* Cygwin (https://www.cygwin.com/) (default install) (packages to add: make, zip, lftp)

Optional:

* Notepad++
    (https://notepad-plus-plus.org/)
* Total commander
    (https://www.ghisler.com/)

    

Installing OpenSSH:

  Settings -> Apps -> Optional features -> Add a feature -> OpenSSH Server -> Install
  Back to Optional features and wait for the install to finish.
  Reboot

Enable the OpenSSH server:

  Right click start menu -> Computer Management -> Services And Applications -> Services
  Find "OpenSSh SSH Server" in the list.
  Double click -> Startup: Automatic
  Click "Start"
  Click "Ok"
    
Use cygwin-bash:

Copy c:\cygwin64\Cygwin.bat to c:\cygwin64\Cygwin-ssh.bat
  @echo off
  setlocal enableextensions
  set TERM=
  cd /d "%~dp0bin" && .\bash --login

New-ItemProperty -Path "HKLM:\SOFTWARE\OpenSSH" -Name DefaultShell -Value "c:\cygwin64\Cygwin-ssh.bat" -PropertyType String -Force    

///////////// Alternative SSH shell setups

Set SSH shell to Powershell:

  New-ItemProperty -Path "HKLM:\SOFTWARE\OpenSSH" -Name DefaultShell -Value "C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe" -PropertyType String -Force
//  New-ItemProperty -Path "HKLM:\SOFTWARE\OpenSSH" -Name DefaultShellCommandOption -Value "/c" -PropertyType String -Force
//  New-ItemProperty -Path "HKLM:\SOFTWARE\OpenSSH" -Name DefaultShellEscapeArguments -Value 1 -PropertyType DWORD -Force  


Use git-bash:

New-ItemProperty -Path "HKLM:\SOFTWARE\OpenSSH" -Name DefaultShell -Value "C:\Program Files\Git\bin\bash.exe" -PropertyType String -Force    

/////////////////////////////////////////

Password less login:
--------------------

For sshd-config do the following changes:

Comment this one:

  #Match Group administrators
  #      AuthorizedKeysFile __PROGRAMDATA__/ssh/administrators_authorized_keys

Uncomment these two:

  PubkeyAuthentication yes

Then open PowerShell as Admin and restart service:
  Restart-Service sshd

Then through File Explorer GUI.
Create the directory C:\Users\<name>\.ssh and C:\Users\<name>\.ssh\authorized_keys
For .ssh folder and authorized_keys file do the following:

  Properties -> Security -> Advanced -> Disable inheritance -> Convert inherited permissions into explicit permissions on this object.

Then delete all users(groups) except:

  System
  Administrators
  current user


Relevant docs (warning: some outdated):

    https://winaero.com/blog/enable-openssh-server-windows-10/

    https://github.com/PowerShell/Win32-OpenSSH/wiki/DefaultShell

    https://docs.microsoft.com/en-us/windows-server/administration/openssh/openssh_overview
    https://docs.microsoft.com/en-us/windows-server/administration/openssh/openssh_server_configuration

    https://github.com/PowerShell/Win32-OpenSSH/issues/1306#issuecomment-484475323  // Passwordless login, see bottom
