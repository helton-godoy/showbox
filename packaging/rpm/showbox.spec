Name:           showbox
Version:        1.0.0
Release:        1%{?dist}
Summary:        GUI widgets for shell scripts

License:        GPL-3.0-or-later
URL:            https://github.com/helton-godoy/showbox
Source0:        %{name}-%{version}.tar.gz

# Build dependencies
BuildRequires:  gcc-c++
BuildRequires:  make
BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-qtcharts-devel
BuildRequires:  qt6-qtsvg-devel

# Runtime dependencies (auto-detected via AutoReqProv)
# But we can specify explicit ones for clarity
Requires:       qt6-qtbase
Requires:       qt6-qtcharts
Requires:       qt6-qtsvg

# Enable automatic dependency detection
AutoReqProv:    yes

%description
ShowBox is a Qt6-based tool that provides GUI widgets for shell scripts.
It allows creating dialogs, forms, and interactive interfaces directly
from bash scripts using a simple command-based syntax.

Features:
- Standard widgets: buttons, labels, checkboxes, text boxes, etc.
- Extended widgets: calendar, table, chart
- Theme support (light/dark/system)
- Resizable windows

%prep
%autosetup

%build
cd src/code/showbox
qmake6 showbox.pro
%make_build

%install
install -D -m 755 src/code/showbox/bin/showbox %{buildroot}%{_bindir}/showbox
install -D -m 644 packaging/rpm/showbox.desktop %{buildroot}%{_datadir}/applications/showbox.desktop

%files
%license LICENSE
%doc README.md
%{_bindir}/showbox
%{_datadir}/applications/showbox.desktop

%changelog
* Sun Jan 12 2026 Helton Godoy <helton@example.com> - 1.0.0-1
- Initial release
- Qt6-based GUI widgets for shell scripts
- Standard and extended widgets support
- Theme support
