#
# spec file for package systray-x-minimal
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

Name:           systray-x-minimal
Version:        0
Release:        0
Summary:        A system tray extension for Thunderbird 68+ (Unity/XFCE/other)
License:        MPL-2.0
Group:          Productivity/Networking/Email/Clients
URL:            https://github.com/Ximi1970/systray-x
Source0:        %{name}-%{version}.tar.xz
Source1:        VERSION
BuildRequires:  unzip
BuildRequires:  zip
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Widgets)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(x11)
%if 0%{?fedora_version} || 0%{?centos_version}
Requires:       thunderbird >= 68
Requires:       thunderbird < 98
%else
Requires:       MozillaThunderbird >= 68
Requires:       MozillaThunderbird < 98
%endif

%description
SysTray-X is a system tray extension for Thunderbird 68+.
This version is for non-KDE and non-GNOME desktops.
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

make %{?_smp_mflags} OPTIONS="DEFINES+=NO_KDE_INTEGRATION" EXT_VERSION="DEFINES+=EXT_VERSION DEFINES+=APP_VERSION_MAJOR=\\\\\\\\\\\\\\\"$VERSION_MAJOR\\\\\\\\\\\\\\\" DEFINES+=APP_VERSION_MINOR=\\\\\\\\\\\\\\\"$VERSION_MINOR\\\\\\\\\\\\\\\" DEFINES+=APP_VERSION_PATCH=\\\\\\\\\\\\\\\"$VERSION_PATCH\\\\\\\\\\\\\\\" DEFINES+=APP_BUILD=\\\\\\\\\\\\\\\"$BUILD_NUMBER\\\\\\\\\\\\\\\" DEFINES+=APP_GITHASH=\\\\\\\\\\\\\\\"$GIT_HASH\\\\\\\\\\\\\\\" DEFINES+=APP_GITBRANCH=\\\\\\\\\\\\\\\"$GIT_BRANCH\\\\\\\\\\\\\\\""

sed < app/config/linux/SysTray_X.json.template -e 's|SYSTRAY_X_PATH|%{_bindir}/SysTray-X|' > SysTray_X.json

%install
install -Dm0755 SysTray-X %{buildroot}/%{_bindir}/SysTray-X
install -Dm0644 SysTray_X.json %{buildroot}%{_libdir}/mozilla/native-messaging-hosts/SysTray_X.json
%if 0%{?fedora_version} || 0%{?centos_version}
# Option 1 (enigmail way) does not work
#_systx_dir=%{buildroot}%{_datadir}/mozilla/extensions/\{3550f703-e582-4d05-9a08-453d09bdfdc6\}/systray-x@Ximi1970
#mkdir -pv $_systx_dir
#unzip -d $_systx_dir systray-x@Ximi1970.xpi

# Option 2 (Ubuntu way) does not work
#install -Dm0644 systray-x@Ximi1970.xpi %{buildroot}/usr/lib/thunderbird-addons/extensions/systray-x@Ximi1970.xpi

# Force into thunderbird dir, system addons not working / not allowed / unknown extensions search path
install -Dm0644 systray-x@Ximi1970.xpi %{buildroot}%{_libdir}/thunderbird/distribution/extensions/systray-x@Ximi1970.xpi
%else
_systx_dir=%{buildroot}%{_libdir}/mozilla/extensions/\{3550f703-e582-4d05-9a08-453d09bdfdc6\}/systray-x@Ximi1970
mkdir -pv $_systx_dir
unzip -d $_systx_dir systray-x@Ximi1970.xpi
%endif

%files
%license LICENSE 
%doc README.md systray-x@Ximi1970.xpi
%{_bindir}/SysTray-X
%{_libdir}/mozilla
%if 0%{?fedora_version} || 0%{?centos_version}
#   Option 1
#%dir /usr/lib/thunderbird-addons
#%dir /usr/lib/thunderbird-addons/extensions
#/usr/lib/thunderbird-addons/extensions/systray-x@Ximi1970.xpi
#   Option 2
#%dir %{_datadir}/mozilla/
#%dir %{_datadir}/mozilla/extensions
#%dir %{_datadir}/mozilla/extensions/\{3550f703-e582-4d05-9a08-453d09bdfdc6\}
#%{_datadir}/mozilla/extensions/\{3550f703-e582-4d05-9a08-453d09bdfdc6\}/systray-x@Ximi1970
%dir %{_libdir}/thunderbird
%dir %{_libdir}/thunderbird/distribution
%dir %{_libdir}/thunderbird/distribution/extensions
%{_libdir}/thunderbird/distribution/extensions/systray-x@Ximi1970.xpi
%endif

%changelog
