Name:           box86
Version:        0.3.0
Release:        1%{?dist}
Summary:        Linux Userspace x86 Emulator

License:        MIT
URL:            https://github.com/ptitSeb/box86
Source0:        %{url}/archive/v%{version}/%{name}-%{version}.tar.gz

# Only relevant on 32bit arm
ExclusiveArch:  %{arm}

# This custom i386 libstdc++ needs to filter requires to be installable on arm
%global __requires_exclude_from ^(%{_prefix}/lib/i386-linux-gnu/.*\\.so.*)$

BuildRequires:  gcc-c++
BuildRequires:  cmake

%description
Linux Userspace x86 Emulator with a twist, targeted at ARM Linux devices.

%prep
%autosetup

# Fix perms
find . -type f -name *.c -exec chmod -x {} ';'
find . -type f -name *.h -exec chmod -x {} ';'
find . -type f -name *.S -exec chmod -x {} ';'
chmod -x LICENSE

%build
%cmake \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DNOGIT=1 \
%if 0%{?_with_dynarec}
  -DARM_DYNAREC=1 \
%endif

%cmake_build


%install
%cmake_install

# Switch to system config and use correct path
mv %{buildroot}%{_sysconfdir}/binfmt.d %{buildroot}%{_prefix}/lib/binfmt.d
sed -i -e 's|/usr/local|/usr|' %{buildroot}%{_prefix}/lib/binfmt.d/box86.conf

%{?_with_tests:
%check
# Tests are failing for now as box86 isn't registered in binfmt.d
%ctest
}

%files
%license LICENSE 
%{_prefix}/lib/binfmt.d/box86.conf
%{_bindir}/box86
%ifnarch %{ix86}
%{_prefix}/lib/i386-linux-gnu/libgcc_s.so.1
%{_prefix}/lib/i386-linux-gnu/libstdc++.so.5
%{_prefix}/lib/i386-linux-gnu/libstdc++.so.6
%{_prefix}/lib/i386-linux-gnu/libpng12.so.0
%endif

%changelog
* Tue Mar 21 2023 Robert Zaage <robert@zaage.it> - 0.3.0-1
- Initial release of version 0.3.0
