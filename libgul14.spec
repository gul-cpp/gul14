#
# This file is used to build RPMS for libgul14
#
# To use it do the following:
#
# 1) create a dist tarball
#   $ meson build && ninja -C build dist
#
# 2) Prepare the rpm build environment
#   $ pkcon install rpmdevtools
#   $ rpmdev-setuptree
#   $ cp build/meson-dist/gul14-<version>.tar.xz ~/rpmbuild/SOURCES
#   $ cp libgul14.spec ~/rpmbuild/SPECS
#
#  On RHEL7 we need a newer compiler
#   $ pkcon install centos-release-scl-rh && pkcon refresh && pkcon install devtoolset-7
#
# 3) Build the package
#   $ cd ~/rpmbuild
#   $ rpmbuild -ba SPECS/libgul14.spec
#  RHEL7 only
#   $ scl enable devtoolset-7 'rpmbuild -ba SPECS/libgul14.spec'
#
# https://rpm-packaging-guide.github.io/
#

Name:           libgul14
Version:        2.6
Release:        1%{?dist}
Summary:        General Utility Library

License:        GPLv2
URL:            https://winweb.desy.de/mcs/docs/gul/
Source0:        gul14-%{version}.tar.xz

%if 0%{?rhel} < 8
BuildRequires:  devtoolset-7-gcc-c++
%endif
BuildRequires:  gcc-c++
BuildRequires:  meson
BuildRequires:  ninja-build
# we actually use some git checks in meson
BuildRequires:  git-core

%description
The general utility library for C++14 contains often-used
utility functions and types that form the foundation for
other libraries and programs.

%package devel
Summary:        Development libraries and header files for %{name}
Requires:       %{name}%{?_isa} = %{version}-%{release}
# We can use BuildRequire:  pkgconfig(gul14)
# in using projects to automatically use either one:
##Requires:       pkgconfig
##Requires:       pkgconf-pkg-config

%description devel
%{summary}.

%package static
Summary:        Static libraries for %{name}
Requires:       %{name}-devel%{?_isa} = %{version}-%{release}

%description static
%{summary}.

%ifarch %{arm} aarch64
# Disable the creation of debug packages on ARM
%global debug_package %{nil}
%endif

%prep
%setup -q -n gul14-%{version}

%build
%meson
%meson_build

%install
%meson_install

%check
%meson_test

%files
%{_libdir}/%{name}.so.*

%files devel
%{_libdir}/%{name}.so
%{_libdir}/pkgconfig/%{name}.pc
%{_includedir}/gul14/*

%files static
%{_libdir}/%{name}.a

%changelog
* Thu Aug 19 2021 Soeren Grunewald <soeren.grunewald@desy.de> - 2.6-1
- Initial release
