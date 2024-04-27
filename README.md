# SysTray-X

SysTray-X is a system tray extension for Thunderbird. The addon uses the WebExtension API's to control an external system dependent system tray application.

The addon and system tray application can do:

- custom new mail icon
- display number of unread / new mails
- optional new mail indicator (icon or shading)
- run app on start or close
- show / hide Thunderbird (minimize)
- use a shortcut to show / hide (not available for GNOME or Wayland)
- minimizing hides to tray (remove Thunderbird from taskbar, pager and switcher when minimized)
- minimize on startup
- minimize on close

Warning:

This add-on and companion app will NOT work with flatpaks or snaps of Thunderbird. They do not allow native messaging hosts outside the packages.

## Donations
To support this project, you can make a donation to its current maintainer:  

[![paypal](https://github.com/Ximi1970/Donate/blob/master/paypal_btn_donateCC_LG_2.gif)](https://paypal.me/Ximi1970)
[![bitcoin-qrcode-black](https://github.com/Ximi1970/Donate/blob/master/bitcoin-donate-qrcode-black.png)](https://raw.githubusercontent.com/Ximi1970/Donate/master/bitcoin-address.txt)


## Contributers

Luigi Baldoni \<aloisio@gmx.com\>		: Initial setup of the OpenSuSE Build Service rpm package.  
ouztheone <github> : Nice workaround without disabling Wayland completely

## Known issues

- Upgrading a Windows version (pre 0.9.4):
    - Quit Thunderbird
    - Install the new version of Systray-X (SysTray-X-setupxx-0.x.0.exe from the release assets)
    - Start Thunderbird again
    - Goto Tools -> Add-ons and Themes
    - Click on the "gear" button / menu
    - Click the "Install Add-on From File" menu item
    - Browse to "C:\Program Files\Systray-X
    - Select "systray-x@Ximi1970.xpi"

- SysTray-X preferences will be lost when moving from an old Thunderbird version to Thunderbird version 91 or higher due to the storage type change. As of SysTray-X version 0.8.0 the add-on will use the local storage type instead of the sync type.  
Please press the reset button in the TB SysTray-X add-on preferences page to clear the old storage before setting your preferences again.

- Preferences menu not working in TB 86.x throught TB 87.0b1 (beta releases)


## Limitations

This add-on / companion app will not work with flatpaks or snaps of Thunderbird. They do not allow native messaging hosts outside the packages.  
You need to use a workaround for Wayland. In some distributions this is already applied.  
The shortcut option does not work in GNOME or Wayland.

## Binaries (System)

These packages will install the Thunderbird addon and companion app system wide.  
Root credentials are required.

Make sure you removed / uninstalled any SysTray-X package installed with a script installer.  
You can verify this by checking the `~/.mozilla/native-messaging-hosts` directory. There should be NO SysTray-X packages or links in there. Remove them and remove the add-on from the Thunderbird add-ons page.


## Using the addon with Wayland

You need to force Thunderbird to use X11.

Starting from a shell:
```bash
env GDK_BACKEND=x11 thunderbird
```

and/or find the .desktop file for Thunderbird:
```
/usr/share/applications/thunderbird.desktop
```
and change the `Exec` line into this:
```
Exec=env GDK_BACKEND=x11 thunderbird %u
```


## Linux distributions

### openSuSE

#### Repository

Installing the repository:

###### Tumbleweed

```bash
sudo zypper ar -f https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/openSUSE_Tumbleweed/ SysTray-X
```

###### Slowroll

```bash
sudo zypper ar -f https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/openSUSE_Slowroll/ SysTray-X
```

###### 15.6

```bash
sudo zypper ar -f https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/15.6/ SysTray-X
```

###### 15.5

```bash
sudo zypper ar -f https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/15.5/ SysTray-X
```

###### 15.4

```bash
sudo zypper ar -f https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/15.4/ SysTray-X
```

###### Leap 15.4

```bash
sudo zypper ar -f https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/openSUSE_Leap_15.4/ SysTray-X
```

#### Package

Installing the SysTray-X addon and companion app package:

###### KDE

```bash
sudo zypper in systray-x
```

###### GNOME

```bash
sudo zypper in systray-x-gnome
```

###### XFCE / Others

```bash
sudo zypper in systray-x-minimal
```


### xUbuntu

#### Repository

Installing the repository:

###### 24.04 LTS

Remove the Thunderbird snap package:
```bash
sudo apt remove thunderbird
```

Install the deb Thunderbird repository:
```bash
sudo add-apt-repository ppa:mozillateam/ppa
sudo nano /etc/apt/preferences.d/mozillateamppa

```

Insert this into the preferences file (it blocks the snap package):
```bash
Package: thunderbird*
Pin: release o=LP-PPA-mozillateam
Pin-Priority: 1001
```
Save the file.

Refresh the repositories and install the deb version of Thunderbird:
```bash
sudo apt update
sudo apt install thunderbird
```

Now install the SysTray-X repository:
```bash
wget -q https://download.opensuse.org/repositories/home:/Ximi1970/xUbuntu_24.04/Release.key
sudo mv -f  Release.key  /etc/apt/trusted.gpg.d/Systray-x.Ximi1970.asc
sudo bash -c 'echo "deb https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/xUbuntu_24.04 ./" > /etc/apt/sources.list.d/systray-x.list'
sudo apt update
```

###### 23.10

```bash
wget -q https://download.opensuse.org/repositories/home:/Ximi1970/xUbuntu_23.10/Release.key
sudo mv -f  Release.key  /etc/apt/trusted.gpg.d/Systray-x.Ximi1970.asc
sudo bash -c 'echo "deb https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/xUbuntu_23.10 ./" > /etc/apt/sources.list.d/systray-x.list'
sudo apt update
```

###### 23.04

```bash
wget -q https://download.opensuse.org/repositories/home:/Ximi1970/xUbuntu_23.04/Release.key
sudo mv -f  Release.key  /etc/apt/trusted.gpg.d/Systray-x.Ximi1970.asc
sudo bash -c 'echo "deb https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/xUbuntu_23.04 ./" > /etc/apt/sources.list.d/systray-x.list'
sudo apt update
```

###### 22.04 LTS

```bash
wget -q https://download.opensuse.org/repositories/home:/Ximi1970/xUbuntu_22.04/Release.key
sudo mv -f  Release.key  /etc/apt/trusted.gpg.d/Systray-x.Ximi1970.asc
sudo bash -c 'echo "deb https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/xUbuntu_22.04 ./" > /etc/apt/sources.list.d/systray-x.list'
sudo apt update
```

###### 20.04 LTS

```bash
wget -q https://download.opensuse.org/repositories/home:/Ximi1970/xUbuntu_20.04/Release.key
sudo apt-key add Release.key
sudo bash -c 'echo "deb https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/xUbuntu_20.04 ./" > /etc/apt/sources.list.d/systray-x.list'
sudo apt update
```

###### 18.04 LTS

```bash
wget -q https://download.opensuse.org/repositories/home:/Ximi1970/xUbuntu_18.04/Release.key
sudo apt-key add Release.key
sudo bash -c 'echo "deb https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/xUbuntu_18.04 ./" > /etc/apt/sources.list.d/systray-x.list'
sudo apt update
```

###### 16.04 LTS

```bash
wget -q http://download.opensuse.org/repositories/home:/Ximi1970/xUbuntu_16.04/Release.key
sudo apt-key add Release.key
sudo bash -c 'echo "deb http://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/xUbuntu_16.04 ./" > /etc/apt/sources.list.d/systray-x.list'
sudo apt update
```

#### Package

Installing the SysTray-X addon and companion app package:

###### KDE

```bash
sudo apt install systray-x
```

###### GNOME

```bash
sudo apt install systray-x-gnome
```

###### Unity / XFCE / Others

```bash
sudo apt install systray-x-minimal
```


### Debian

#### Repository

Installing the repository:

###### Testing

```bash
wget -q https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/Debian_Testing/Release.key
sudo cp -f Release.key /etc/apt/trusted.gpg.d/SysTray-X.asc
sudo bash -c 'echo "deb https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/Debian_Testing ./" > /etc/apt/sources.list.d/systray-x.list'
sudo apt update
```

###### Unstable

```bash
wget -q https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/Debian_Unstable/Release.key
sudo cp -f Release.key /etc/apt/trusted.gpg.d/SysTray-X.asc
sudo bash -c 'echo "deb https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/Debian_Unstable ./" > /etc/apt/sources.list.d/systray-x.list'
sudo apt update
```

###### 12

```bash
wget -q https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/Debian_12/Release.key
sudo cp -f Release.key /etc/apt/trusted.gpg.d/SysTray-X.asc
sudo bash -c 'echo "deb https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/Debian_12 ./" > /etc/apt/sources.list.d/systray-x.list'
sudo apt update
```

###### 11

```bash
wget -q https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/Debian_11/Release.key
sudo apt-key add Release.key
sudo bash -c 'echo "deb https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/Debian_11 ./" > /etc/apt/sources.list.d/systray-x.list'
sudo apt update
```

###### 10

```bash
wget -q https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/Debian_10/Release.key
sudo apt-key add Release.key
sudo bash -c 'echo "deb https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/Debian_10 ./" > /etc/apt/sources.list.d/systray-x.list'
sudo apt update
```

#### Package

Installing the SysTray-X addon and companion app package:

###### KDE

```bash
sudo apt install systray-x
```

###### GNOME

```bash
sudo apt install systray-x-gnome
```

###### XFCE / Others

```bash
sudo apt install systray-x-minimal
```


### Fedora

#### Repository

Installing the repository:

###### Rawhide

```bash
sudo dnf config-manager --add-repo https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/Fedora_Rawhide/home:Ximi1970:Mozilla:Add-ons.repo
sudo dnf update --refresh
```

###### 40

```bash
sudo dnf config-manager --add-repo https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/Fedora_40/home:Ximi1970:Mozilla:Add-ons.repo
sudo dnf update --refresh
```

###### 39

```bash
sudo dnf config-manager --add-repo https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/Fedora_39/home:Ximi1970:Mozilla:Add-ons.repo
sudo dnf update --refresh
```

###### 38

```bash
sudo dnf config-manager --add-repo https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/Fedora_38/home:Ximi1970:Mozilla:Add-ons.repo
sudo dnf update --refresh
```

###### 37

```bash
sudo dnf config-manager --add-repo https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/Fedora_37/home:Ximi1970:Mozilla:Add-ons.repo
sudo dnf update --refresh
```

###### 36

```bash
sudo dnf config-manager --add-repo https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/Fedora_36/home:Ximi1970:Mozilla:Add-ons.repo
sudo dnf update --refresh
```

#### Package

Installing the SysTray-X addon and companion app package:

###### KDE

```bash
sudo dnf install systray-x
```

###### GNOME

```bash
sudo dnf install systray-x-gnome
```

###### XFCE / Others

```bash
sudo dnf install systray-x-minimal
```


### Manjaro, Arch

#### AUR

#### Repository

Installing the repository:

```bash
wget -q https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons:/Arch/Arch/x86_64/home_Ximi1970_Mozilla_Add-ons_Arch_Arch.key
sudo pacman-key --add home_Ximi1970_Mozilla_Add-ons_Arch_Arch.key
sudo pacman-key --lsign-key BEEF5C3607D86FE9
sudo echo -e "\n[home_Ximi1970_Mozilla_Add-ons_Arch_Arch]\nSigLevel = PackageOptional\nServer = https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons:/Arch/Arch/x86_64" | sudo tee -a /etc/pacman.conf 
sudo pacman -Syyu
```

#### Package

Installing the SysTray-X addon and companion app package:

###### KDE

```bash
sudo pacman -S systray-x
```

###### GNOME

```bash
sudo pacman -S systray-x-gnome
```

Please use `gnome-extensions` to enable the gnome shell extension `appindicatorsupport@rgcjonas.gmail.com`. Reboot or relogin is probably also needed.

###### XFCE / Others

```bash
sudo pacman -S systray-x-minimal
```

#### AUR alternative install (by Antiz96)

Install the `systray-x` package from the Arch repo.
This is a split package that will offer you to either install the [systray-x-common](https://archlinux.org/packages/extra/x86_64/systray-x-common/) package which is suitable for any DE/WM except KDE (Gnome users need to install and enable the `gnome-shell-extension-appindicator` for a proper integration with Gnome) or the [systray-x-kde](https://archlinux.org/packages/extra/x86_64/systray-x-kde/) package which includes specific options and dependencies for a proper integration with KDE.

```bash
sudo pacman -S systray-x
```

You can directly install the `systray-x-common` or the `systray-x-kde` package instead, if you already know the one you want to install.
Alternatively, there's a [systray-x-git](https://aur.archlinux.org/packages/systray-x-git) package available in the AUR.


### MX

First install the Debian latest Thunderbird version. Replace the xx.x.x with the correct version.
```bash
sudo apt install thunderbird=1:xx.x.x-1~deb10u1
```

Then add the SysTray-X Debian repository and install the SysTray-X package.

```bash
sudo apt install thunderbird=1:78.x.x-1~deb10u1
wget -q https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/Debian_10/Release.key
sudo apt-key add Release.key
sudo bash -c 'echo "deb https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/Debian_10 ./" > /etc/apt/sources.list.d/systray-x.list'
sudo apt update
sudo apt install systray-x-minimal
```


## Binaries (User) (not recommended)

User installable package.  
No root credentials required.  

Download the installer here (click on the assets dropdown list):

[Addon and companion app installer](https://github.com/Ximi1970/systray-x/releases)


Please keep in mind that using the script will override the SysTray-X packages installed with a repository.
Use the `-u` option with the script installer to uninstall.


## Building

### Linux

Requirements:
  - OpenSuSE Leap 15.5:

    ```bash
    sudo zypper install zip git gcc-c++ make libQt5Core-devel libQt5Widgets-devel libQt5DBus-devel knotifications-devel
    ```

  - OpenSuSE Leap 15.5 GNOME:

    ```bash
    sudo zypper install zip git gcc-c++ make libQt5Core-devel libQt5Widgets-devel libQt5DBus-devel
    sudo zypper ar -f https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/15.5/ SysTray-X
    sudo zypper install gnome-shell-extension-appindicator
    ```

  - Fedora 38:

    ```bash
    sudo dnf install qt5-qtbase-devel gnome-shell-extension-appindicator
    gnome-extensions enable appindicatorsupport@rgcjonas.gmail.com
    ```

    Logout and login to enable the gnome extension.

  - Fedora 38 Mate:

    ```bash
    sudo dnf install qt5-qtbase-devel kf5-knotifications-devel
    ```

  - Ubuntu 23.04 LTS:

    ```bash
    sudo apt install zip git g++ make qt5-qmake qtbase5-dev qdbus-qt5
    gnome-extensions enable ubuntu-appindicator@ubuntu.com
    ```

    Logout and login to enable the gnome extension.

  - Debian 12:

    ```bash
    sudo apt install zip git g++ make qt5-qmake qtbase5-dev qdbus-qt5 gnome-shell-extension-appindicator
    gnome-extension enable ubuntu-appindicator@ubuntu.com
    ```

    Logout and login to enable the gnome extension.


Clone the repository:
```bash
git clone https://github.com/Ximi1970/systray-x.git
```

Build (KDE):
```bash
cd systray-x
make clean
make
```
or if `knotifications` is not available:
```bash
cd systray-x
make clean
make OPTIONS="DEFINES+=NO_KDE_INTEGRATION"
```


Build (GNOME,XFCE,others):
```bash
cd systray-x
make clean
make OPTIONS="DEFINES+=NO_KDE_INTEGRATION"
```


Install and use SysTray-X from the build directory in the repository:
```bash
mkdir -p ~/.mozilla/native-messaging-hosts
cp -f app/config/linux/SysTray_X.json ~/.mozilla/native-messaging-hosts/
```

Copy the add-on into the Thunderbird profile directory:
```bash
cp -f systray-x@Ximi1970.xpi ~/.thunderbird/<profile-code>.default-release/extensions/
```

Start Thunderbird and enable and configure the add-on in the "Menu -> Add-ons and themes -> Extensions" dialog.

Or install the add-on by going to the "Add-ons and themes -> Extensions" dialog, click on the gear icon and use the "Install add-on from file" item. Select the "systray-x@Ximi1970.xpi" file.


### Windows 10, 11

Install these packages:
- [Visual Studio Express 2019](https://visualstudio.microsoft.com/vs/older-downloads/)
- [Visual Code Studio](https://code.visualstudio.com/)
- [Git for Windows](https://gitforwindows.org/)
- [TortoiseGit](https://tortoisegit.org/)
- [PuTTY](https://www.putty.org/)
- [Qt](https://www.qt.io/download-thank-you?os=windows&hsLang=en) (QtCreator, Qt 5.15.2)
- [7z](https://www.7-zip.org/download.html)
- (optional)[Cygwin](https://www.cygwin.com/) (gcc, make, zip)

Clone the repository using TortoiseGit.

Start QtCreator and open the project file:
```
app/SysTray-X/SysTray-X.pro
```

Choose in the `Projects` the kit `Desktop Qt 5.12.2 MSVC2019 64bit`
Set it to `Release` or `Debug` on what you need.
Then `Build all projects`.

Now create the `systray-x@Ximi1970.xpi`
```bash
cd webext
"C:\Program Files\7-Zip\7z.exe" a ../systray-x@Ximi1970.xpi *
```

Install `app\config\win32\SysTray-X-User.reg` to activated the addon for only the current user.
Please remove this registry entry if you ever consider to install the addon with the official installer.

Install `app\config\win32\SysTray-X-System.reg` to activated the addon for every one.

You can now install the addon in Thunderbird.
```
Menu -> Add-ons and Themes -> Click the gear symbol -> Install add-on from file... -> Select systray-x@Ximi1970.xpi
```


### Translations

de      :  
el      :   askembris  
en_US   :  
it      :   lgbaldoni  
nl      :  
pt_BR   :   fabianski7  
ru_RU   :   agvantibo  
zh_CN   :	UM-Li


### Remarks

Branches:  

master      : Should be stable and working for all supported systems  
develop     : Working but maybe not for all systems  
feature-xxx : Experimental  
