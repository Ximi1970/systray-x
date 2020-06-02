#
# spec file for package systray-x-gnome
#
# Copyright (c) 2020 SUSE LLC
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via https://bugs.opensuse.org/
#

Name:           systray-x-gnome
Version:        0
Release:        0
Summary:        A system tray extension for Thunderbird 68+
License:        MPL-2.0
Group:          Productivity/Networking/Email/Clients
URL:            https://github.com/Ximi1970/systray-x
Source0:        %{name}-%{version}.tar.xz
Source1:        VERSION
BuildRequires:  unzip
BuildRequires:  zip
BuildRequires:  gcc-c++
BuildRequires:  fontconfig-devel
%if 0%{?fedora_version}
BuildRequires:  freetype-devel
%else
BuildRequires:  freetype2-devel
%endif
BuildRequires:  libX11-devel
BuildRequires:  libxkbcommon-x11-devel
BuildRequires:  libqt5-opensource-5.14.2-minimal-static
Requires:       gnome-shell-extension-appindicator
%if 0%{?fedora_version}
Requires:       thunderbird >= 68
%else
Requires:       MozillaThunderbird >= 68
%endif

%description
SysTray-X is a system tray extension for Thunderbird 68+.
The add-on uses the WebExtension API's to control an external system
dependent system tray application.

The add-on and system tray application can do:

  * custom new mail icon
  * display number of unread mails
  * show / hide Thunderbird (minimize)
  * minimizing hides to tray
  * minimize on startup
  * minimize on close

%prep
%autosetup -p1

%build
export VERSION=`cat %{S:1} | grep VERSION | sed -e "s/VERSION=\(.*\)/\1/"`

export VERSION_MAJOR=`echo $VERSION | cut -d'.' -f1`
export VERSION_MINOR=`echo $VERSION | cut -d'.' -f2`
export VERSION_PATCH=`echo $VERSION | cut -d'.' -f3`

export BUILD_NUMBER=`cat %{S:1} | grep BUILD_NUMBER | sed -e "s/BUILD_NUMBER=\(.*\)/\1/"`
export GIT_HASH=`cat %{S:1} | grep GIT_HASH | sed -e "s/GIT_HASH=\(.*\)/\1/"`
export GIT_BRANCH=`cat %{S:1} | grep GIT_BRANCH | sed -e "s/GIT_BRANCH=\(.*\)/\1/"`

make %{?_smp_mflags} all-static EXT_VERSION="DEFINES+=EXT_VERSION DEFINES+=APP_VERSION_MAJOR=\\\\\\\\\\\\\\\"$VERSION_MAJOR\\\\\\\\\\\\\\\" DEFINES+=APP_VERSION_MINOR=\\\\\\\\\\\\\\\"$VERSION_MINOR\\\\\\\\\\\\\\\" DEFINES+=APP_VERSION_PATCH=\\\\\\\\\\\\\\\"$VERSION_PATCH\\\\\\\\\\\\\\\" DEFINES+=APP_BUILD=\\\\\\\\\\\\\\\"$BUILD_NUMBER\\\\\\\\\\\\\\\" DEFINES+=APP_GITHASH=\\\\\\\\\\\\\\\"$GIT_HASH\\\\\\\\\\\\\\\" DEFINES+=APP_GITBRANCH=\\\\\\\\\\\\\\\"$GIT_BRANCH\\\\\\\\\\\\\\\""

sed < app/config/linux/SysTray_X.json.template -e 's|SYSTRAY_X_PATH|%{_bindir}/SysTray-X|' > SysTray_X.json

%install
_systx_dir=%{buildroot}%{_libdir}/mozilla/extensions/\{3550f703-e582-4d05-9a08-453d09bdfdc6\}/systray-x@Ximi1970
install -Dm0755 SysTray-X %{buildroot}/%{_bindir}/SysTray-X
mkdir -pv $_systx_dir
unzip -d $_systx_dir systray-x@Ximi1970.xpi
install -Dm0644 SysTray_X.json %{buildroot}%{_libdir}/mozilla/native-messaging-hosts/SysTray_X.json

%post
PROF_DIR="/etc/dconf/profile"
PROF_FILE="user"

if [ -f ${PROF_DIR}/${PROF_FILE} ] ; then
    #
    # Edit user file
    #
    grep -q "user-db:user" ${PROF_DIR}/${PROF_FILE}
    if [ "$?" = "1" ] ; then
        echo "user-db:user" >> ${PROF_DIR}/${PROF_FILE}
    fi
        
    grep -q "system-db:local" ${PROF_DIR}/${PROF_FILE}
    if [ "$?" = "1" ] ; then
        echo "system-db:local" >> ${PROF_DIR}/${PROF_FILE}
    fi
else
    #
    # Generate user file
    #
    mkdir -p ${PROF_DIR}
    cat >${PROF_DIR}/${PROF_FILE} <<EOF
user-db:user
system-db:local
EOF
fi

EXTENSION="appindicatorsupport@rgcjonas.gmail.com"
CONF_DIR="/etc/dconf/db/local.d"
CONF_FILE="00-extensions"

if [ -f ${CONF_DIR}/${CONF_FILE} ] ; then
  #
  # Edit extensions file
  #
  grep -q ${EXTENSION} ${CONF_DIR}/${CONF_FILE}
  if [ "$?" == "1" ] ; then    
    sed -i -e "s/\(enabled-extensions=\[.*\)\]/\1, '${EXTENSION}'\]/" ${CONF_DIR}/${CONF_FILE}
  fi
else
  #
  # Generate extensions file
  #
  mkdir -p ${CONF_DIR}
  cat >${CONF_DIR}/${CONF_FILE} <<EOF
[org/gnome/shell]
# List all extensions that you want to have enabled for all users
enabled-extensions=['appindicatorsupport@rgcjonas.gmail.com']
EOF
fi
#
if [ -x /usr/bin/dconf ] ; then
  dconf update
fi

%files
%license LICENSE 
%doc README.md systray-x@Ximi1970.xpi
%{_bindir}/SysTray-X
%{_libdir}/mozilla

%changelog
