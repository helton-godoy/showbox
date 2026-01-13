# ShowBox Makefile
# Build, test, and package automation

.PHONY: all build clean install test help
.PHONY: dev start-dev
.PHONY: pkg-all pkg-deb pkg-deb-ubuntu pkg-deb-debian pkg-appimage pkg-rpm pkg-flatpak pkg-dmg pkg-msix

# Default target
all: build

# ============================================================================
# Development
# ============================================================================

build:  ## Compile ShowBox
	cd src/code/showbox && qmake6 && make -j$$(nproc)

clean:  ## Clean build artifacts
	cd src/code/showbox && make clean || true
	rm -f src/code/showbox/bin/showbox
	rm -rf dist/

install: build  ## Install ShowBox to system
	cd src/code/showbox && sudo make install

dev: start-dev  ## Alias for start-dev

start-dev:  ## Start Docker development environment
	./src/start-dev.sh

start-dev-build:  ## Start dev and compile inside container
	./src/start-dev.sh bash -c "cd src/code/showbox && qmake6 && make -j\$$(nproc)"

# ============================================================================
# Testing
# ============================================================================

test:  ## Run tests
	@echo "Running examples..."
	./examples/showbox_demo.sh || true

test-icons:  ## Test icons demo
	./examples/showbox_icons.sh

# ============================================================================
# Packaging
# ============================================================================

pkg-all: pkg-deb-ubuntu pkg-deb-debian pkg-appimage pkg-rpm pkg-flatpak  ## Build all packages

pkg-deb: pkg-deb-ubuntu  ## Build DEB package (default: Ubuntu)

pkg-deb-ubuntu:  ## Build DEB for Ubuntu 24.04
	./packaging/deb/start-pkg-deb.sh ubuntu

pkg-deb-debian:  ## Build DEB for Debian 13
	./packaging/deb/start-pkg-deb.sh debian

pkg-appimage:  ## Build AppImage
	./packaging/appimage/start-pkg-appimage.sh

pkg-rpm:  ## Build RPM package (Fedora/RHEL)
	./packaging/rpm/start-pkg-rpm.sh

pkg-flatpak:  ## Build Flatpak
	./packaging/flatpak/start-pkg-flatpak.sh

pkg-dmg:  ## Build DMG (macOS)
	./packaging/dmg/start-pkg-dmg.sh

pkg-msix:  ## Build MSIX (Windows)
	./packaging/msix/start-pkg-msix.sh

# ============================================================================
# Help
# ============================================================================

help:  ## Show this help
	@echo "ShowBox Makefile"
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[36m%-20s\033[0m %s\n", $$1, $$2}'
