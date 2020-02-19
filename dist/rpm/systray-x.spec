#
# spec file for package systray-x
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


Name:           systray-x
Version:        0
Release:        0
Summary:        A system tray extension for Thunderbird 68+
License:        MPL-2.0
URL:            https://github.com/Ximi1970/systray-x
Source0:        %{name}-%{version}.tar.xz
BuildRequires:  unzip
BuildRequires:  zip
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Widgets)
Requires:       MozillaThunderbird >= 68

%description
SysTray-X is a system tray extension for Thunderbird 68+.
The add-on uses the WebExtension API's to control an external system
dependent system tray application.

The add-on and system tray application can do:

  * custom new mail icon
  * display number of unread mails
  * show / hide Thunderbird (minimize)
  * to be implemented: remove from task bar when minimized
  * to be implemented: start a new mail
  * to be implemented: open the last used account

%prep
%autosetup -p1

%build
make %{?_smp_mflags}

%install
_systx_dir=%{buildroot}%{_libdir}/mozilla/extensions/\{3550f703-e582-4d05-9a08-453d09bdfdc6\}/systray-x@Ximi1970
install -Dm0755 SysTray-X %{buildroot}/%{_bindir}/SysTray-X
mkdir -pv $_systx_dir
unzip -d $_systx_dir systray-x.xpi

%files
%license LICENSE
%doc README.md
%{_bindir}/SysTray-X
%{_libdir}/mozilla

%changelog
