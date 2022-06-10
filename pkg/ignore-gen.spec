Name:           ignore-gen
Version:        0.0.1
Release:        1%{?dist}
Summary:        Offline .gitignore generator based on gitignore.io

License:        MIT
URL:            https://github.com/iAmSomeone2/ignore-gen
Source0:        

BuildRequires:  meson >= 0.56
BuildRequires:  ninja-build >= 1.10
BuildRequires:  gcc >= 7
BuildRequires:  fmt-devel >= 8.1
BuildRequires:  sqlite-devel >= 3.36

Requires:       fmt >= 8.1
Requires:       sqlite >= 3.36

%description
Offline .gitignore generator based on gitignore.io

%prep
%autosetup


%build
%meson -Dbuildtype=release
%meson_build


%install
%meson_install


%files
# %license add-license-file-here
%{_bindir}/${name}
%{_datadir}/${name}/ignore.db


%changelog
* Fri Jun 10 2022 Brenden Davidson <davidson.brenden15@gmail.com>
- 
