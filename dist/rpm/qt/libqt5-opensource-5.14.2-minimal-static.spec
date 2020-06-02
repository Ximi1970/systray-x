#
# spec file for package libqt5-opensource-xxx-minimal-static
#
%define	license_type	 opensource
%define	package_version	 %{nil}


Version:        5.14.2
Name:           libqt5-%{license_type}-%{version}-minimal-static

BuildRequires:  gcc-c++
BuildRequires:  pkgconfig
BuildRequires:  fdupes

BuildRequires:  fontconfig-devel
BuildRequires:  openssl-devel
%if 0%{?fedora_version}
BuildRequires:  freetype-devel
%else
BuildRequires:  freetype2-devel
%endif

BuildRequires:  libxcb-devel
BuildRequires:  libxkbcommon-x11-devel

BuildRequires:  libX11-devel
BuildRequires:  libXcursor-devel
#BuildRequires:	-post-build-checks

Url:            http://qt-project.org/
Summary:        Qt is a cross-platform application and UI framework
License:        GPL-2.0 or GPL-3.0 or LGPL-2.1 or LGPL-3.0 or GFDL-1.3
Group:          System/Libraries
Release:        0
Source:         qt-everywhere-src-%{version}%{package_version}.tar.xz
# to get mtime of file:
Source1:        %{name}.changes
#Patch0:         qt-everywhere-src-%{version}.diff
Patch10:       qt-everywhere-src-%{version}-shebang.diff
%if 0%{?fedora_version}
Patch100:       qt-everywhere-src-%{version}-fedora_shebang.diff
%endif
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
Prefix:         /usr


%define common_options    -%{license_type} -confirm-license -prefix %{prefix}/Qt-%{license_type}-%{version}-static -libdir %{prefix}/Qt-%{license_type}-%{version}-static/%{_libdir} -no-rpath
%define lib_options       -static -fontconfig -sql-sqlite -no-sql-odbc -system-freetype -qt-zlib -qt-libpng -qt-libjpeg -openssl-linked -qt-xcb
%define special_options   -verbose
%define x11_options       %{nil}
%define disable_options	  -no-compile-examples -nomake tests -nomake examples -no-opengl -no-feature-concurrent -no-feature-xml -no-feature-testlib -skip qt3d -skip qtactiveqt -skip qtandroidextras -skip qtcanvas3d -skip qtcharts -skip qtconnectivity -skip qtdatavis3d -skip qtdeclarative -skip qtdoc -skip qtgamepad -skip qtgraphicaleffects -skip qtimageformats -skip qtlocation -skip qtmacextras -skip qtmultimedia -skip qtnetworkauth -skip qtpurchasing -skip qtquickcontrols -skip qtquickcontrols2 -skip qtremoteobjects -skip qtscript -skip qtscxml -skip qtsensors -skip qtserialbus -skip qtserialport -skip qtsvg -skip qtspeech -skip qttools -skip qttranslations -skip qtvirtualkeyboard -skip qtwayland -skip qtwebchannel -skip qtwebsockets -skip qtwebview -skip qtwinextras -skip qtx11extras -skip qtxmlpatterns -skip qtwebengine


%description
Qt is a cross-platform application and UI framework.

This packages contains the full toolkit.


%prep
%setup -q -n qt-everywhere-src-%{version}
#patch0 -p1
%patch10 -p1
#
%if 0%{?fedora_version}
%patch100 -p1
%endif


%build
./configure %common_options %lib_options %special_options %x11_options %disable_options
make %{?jobs:-j %jobs}


%install
make INSTALL_ROOT=$RPM_BUILD_ROOT install

find $RPM_BUILD_ROOT/%{prefix}/Qt-%{license_type}-%{version}-static -type f -name '*la' -print -exec perl -pi -e 's,-L%{_builddir}/\S+,,g' {} \;
find $RPM_BUILD_ROOT/%{prefix}/Qt-%{license_type}-%{version}-static -type f -name '*pc' -print -exec perl -pi -e "s, -L$RPM_BUILD_DIR/?\S+,,g" {} \; -exec sed -i -e "s,^moc_location=.*,moc_location=%_bindir/moc," -e "s,uic_location=.*,uic_location=%_bindir/uic," {} \;

%fdupes $RPM_BUILD_ROOT/%{prefix}/Qt-%{license_type}-%{version}-static


%clean
rm -rf $RPM_BUILD_ROOT


%post -p /sbin/ldconfig


%postun -p /sbin/ldconfig


%files
%defattr(-,root,root,755)
%doc README LICENSE.*
%{prefix}/Qt-%{license_type}-%{version}-static


%changelog
