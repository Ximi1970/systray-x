# SysTray-X

SysTray-X is a system tray extension for Thunderbird 68+. The addon uses the WebExtension API's to control an external system dependent system tray application.  

The addon and system tray application can do:

- custom new mail icon
- display number of unread mails
- show / hide Thunderbird (minimize)
- minimizing hides to tray
- minimize on startup

The default poll time is 60 seconds. You change the timing in the preferences but be aware that a too short poll time can lead to a high CPU load.

## Binaries (System)

These packages will install the Thunderbird addon and companion app system wide.  
Root credentials are required.

### openSuSE
#### Leap 15.1

```bash
sudo zypper ar -f https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/openSUSE_Leap_15.1/ SysTray-X
sudo zypper in systray-x
```

#### Leap 15.2

```bash
sudo zypper ar -f https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/openSUSE_Leap_15.2/ SysTray-X
sudo zypper in systray-x
```

#### Tumbleweed

```bash
sudo zypper ar -f https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/openSUSE_Tumbleweed/ SysTray-X
sudo zypper in systray-x
```

#### SLE 15

```bash
sudo zypper ar -f https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/openSUSE_SLE_15_SP1/ SysTray-X
sudo zypper in systray-x
```

### xUbuntu

#### 19.10

```bash
wget -q https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/xUbuntu_19.10/Release.key
sudo apt-key add Release.key
sudo bash -c 'echo "deb https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/xUbuntu_19.10 ./" > /etc/apt/sources.list.d/systray-x.list'
sudo apt update
sudo apt install systray-x
```

#### 18.04 LTS

```bash
wget -q https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/xUbuntu_18.04/Release.key
sudo apt-key add Release.key
sudo bash -c 'echo "deb https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/xUbuntu_18.04 ./" > /etc/apt/sources.list.d/systray-x.list'
sudo apt update
sudo apt install systray-x
```

### Debian

#### 10

```bash
wget -q https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/Debian_10/Release.key
sudo apt-key add Release.key
sudo bash -c 'echo "deb https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/Debian_10 ./" > /etc/apt/sources.list.d/systray-x.list'
sudo apt update
sudo apt install systray-x
```

### Fedora

#### 31

```bash
sudo dnf config-manager --add-repo https://download.opensuse.org/repositories/home:/Ximi1970:/Mozilla:/Add-ons/Fedora_31/home:Ximi1970:Mozilla:Add-ons.repo
sudo dnf update --refresh
sudo dnf install systray-x
```


## Binaries (User)

User installable package.  
No root credentials required.  

Download the installer here (click on the assets dropdown list):

[Addon and companion app installer](https://github.com/Ximi1970/systray-x/releases)


## Building

### Linux

Clone the repository:
```bash
git clone https://github.com/Ximi1970/systray-x.git
```

Requirements:
  - OpenSuSE:

    ```bash
    sudo zypper install zip git gcc-c++ make qt5-qtbase-devel
    ```

  - Fedora/Centos/RHEL:

    ```bash
    sudo dnf install qt5-qtbase-devel gnome-shell-extension-appindicator
    gnome-extensions enable appindicatorsupport@rgcjonas.gmail.com
    ```
    
  - Ubuntu:

      19.10

    ```bash
    sudo apt install zip git g++ make qt5-default gnome-shell-extension-appindicator
    gnome-extensions enable ubuntu-appindicator@ubuntu.com
    ```

      18.04 LTS

    ```bash
    sudo apt install zip git g++ make qt5-default gnome-shell-extension-appindicator
    gnome-shell-extension-tool -e ubuntu-appindicator@ubuntu.com
    ```

  - Debian:

    ```bash
    sudo apt install zip git g++ make qt5-default gnome-shell-extension-appindicator
    gnome-shell-extension-tool -e ubuntu-appindicator@ubuntu.com
    ```

Build:
```bash
cd systray-x
make
```

Install:
```bash
mkdir -p ~/.mozilla/native-messaging-hosts
cp -f app/config/linux/SysTray_X.json ~/.mozilla/native-messaging-hosts/
cp -f systray-x@Ximi1970.xpi ~/.thunderbird/*.default-release/extensions/
```
...


### Windows

Install:
- [Visual Studio Express 2017](https://aka.ms/vs/15/release/vs_WDExpress.exe)
- [Visual Code Studio](https://code.visualstudio.com/)
- [Git for Windows](https://gitforwindows.org/)
- [TortoiseGit](https://tortoisegit.org/)
- [PuTTY](https://www.putty.org/)
- [Qt](https://www.qt.io/download-thank-you?os=windows&hsLang=en)

Clone the repository using TortoiseGit:

...


### Remarks

Branches:  

master      : Should be stable and working for all supported systems  
develop     : Working but maybe not for all systems  
feature-xxx : Experimental  

## Contributers

Luigi Baldoni \<aloisio@gmx.com\>		: Initial setup of the OpenSuSE Build Service rpm package.



## Donations
To support this project, you can make a donation to its current maintainer:  

[![paypal](https://github.com/Ximi1970/Donate/blob/master/paypal_btn_donateCC_LG_2.gif)](https://paypal.me/Ximi1970)
[![bitcoin-qrcode-black](https://github.com/Ximi1970/Donate/blob/master/bitcoin-donate-qrcode-black.png)](https://raw.githubusercontent.com/Ximi1970/Donate/master/bitcoin-address.txt)
