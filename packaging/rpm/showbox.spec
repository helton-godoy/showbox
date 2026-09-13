Name:           showbox
Version:        @@RPM_VERSION@@
Release:        @@RPM_RELEASE@@%{?dist}
Summary:        GUI widgets for shell scripts

License:        GPL-3.0-or-later
URL:            https://github.com/helton-godoy/showbox
Source0:        %{name}-%{version}.tar.gz

# Build dependencies
BuildRequires:  gcc-c++
BuildRequires:  cmake
BuildRequires:  git
BuildRequires:  python3 >= 3.9
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

# Falhar explicitamente quando algum arquivo instalado ficar sem dono.
%define _unpackaged_files_terminate_build 1

%description
ShowBox is a Qt6-based tool that provides GUI widgets for shell scripts.
It allows creating dialogs, forms, and interactive interfaces directly
from bash scripts using a simple command-based syntax.

Features:
- Standard widgets: buttons, labels, checkboxes, text boxes, etc.
- Extended widgets: calendar, table, chart
- Theme support (light/dark/system)
- Resizable windows

%package studio
Summary: Visual editor for ShowBox interfaces
Requires: showbox

%description studio
Showbox Studio is a visual editor for designing ShowBox dialogs and
interfaces for the showbox engine. It provides a canvas, property
editor, action editor and project files (sbp format).

%prep
%autosetup

%build
%cmake
%cmake_build

%install
%cmake_install

%files
%license packaging/deb/debian/copyright
%doc README.md
%{_bindir}/showbox
%{_datadir}/applications/showbox.desktop

%files studio
%{_bindir}/showbox-studio
%{_bindir}/showbox-studioctl
%{_bindir}/showbox-studio-mcp
%{_datadir}/applications/showbox-studio.desktop
%{_datadir}/icons/hicolor/scalable/apps/showbox-studio.svg

%changelog
* Mon Jan 12 2026 Helton Godoy <helton@example.com> - @@RPM_VERSION@@-@@RPM_RELEASE@@
- Initial release
- Qt6-based GUI widgets for shell scripts
- Standard and extended widgets support
- Theme support
