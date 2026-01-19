# ShowBox Makefile
# Build, test, and package automation

.PHONY: all build clean install test help
.PHONY: dev start-dev start-dev-build
.PHONY: lint format docs check-deps
.PHONY: pkg-all pkg-deb pkg-deb-ubuntu pkg-deb-debian pkg-appimage pkg-rpm pkg-flatpak pkg-dmg pkg-msix

# Variables
QMAKE := qmake6
MAKE := make
NPROC := $(shell nproc)
BUILD_DIR := src/code/showbox
BIN_DIR := $(BUILD_DIR)/bin

# Default target
all: docker-check-deps build

# ============================================================================
# Docker Integration
# ============================================================================

DOCKER_IMAGE := showbox-dev:latest
IS_DOCKER := $(shell [ -f /.dockerenv ] && echo 1 || echo 0)

# Check if we are running inside docker
ifeq ($(wildcard /.dockerenv),)
    # NOT inside Docker
    IN_DOCKER := 0
else
    # Inside Docker
    IN_DOCKER := 1
endif

# Wrapper to run make inside Docker
# Usage: $(call IN_DOCKER_WRAPPER, target_name)
define IN_DOCKER_WRAPPER
	@if [ "$(IN_DOCKER)" = "0" ]; then \
		echo "Detected host environment. Switching to Docker container..."; \
		./src/start-dev.sh make $(1); \
	else \
		$(MAKE) $(1)_internal; \
	fi
endef

docker-build:  ## Build the Docker image
	@echo "Building Docker image $(DOCKER_IMAGE)..."
	docker build --pull -t $(DOCKER_IMAGE) -f src/dev.Dockerfile .

docker-shell:  ## Open a shell in the Docker container
	./src/start-dev.sh

# ============================================================================
# Development
# ============================================================================

build:  ## Compile ShowBox (runs inside Docker)
	$(call IN_DOCKER_WRAPPER,build)

build_internal: check-deps ## Internal build target
	@echo "Building ShowBox (Inside Docker)..."
	cd $(BUILD_DIR) && $(QMAKE) && $(MAKE) -j$(NPROC)

clean:  ## Clean build artifacts (runs inside Docker)
	$(call IN_DOCKER_WRAPPER,clean)

clean_internal: ## Internal clean target
	@echo "Cleaning build artifacts..."
	cd $(BUILD_DIR) && $(MAKE) clean || true
	rm -f $(BIN_DIR)/showbox
	rm -rf dist/
	find . -name "*.o" -delete
	find . -name "moc_*" -delete

install: build  ## Install ShowBox to system (Host only usually, but allowed in docker for testing)
	$(call IN_DOCKER_WRAPPER,install)

install_internal: build_internal
	cd $(BUILD_DIR) && sudo $(MAKE) install

dev: docker-shell  ## Alias for docker-shell



# ============================================================================
# Quality Assurance
# ============================================================================

# Helper for dependency checking
CHECK_DEP = command -v $(1) >/dev/null || (echo "Error: '$(1)' not found. Please install it (e.g., sudo apt install $(2))" && exit 1)
WARN_DEP = command -v $(1) >/dev/null || echo "Warning: '$(1)' not found. Some targets may not work."

docker-check-deps:
	@if [ "$(IN_DOCKER)" = "0" ]; then \
		command -v docker >/dev/null || (echo "Error: docker not found." && exit 1); \
	else \
		$(MAKE) check-deps; \
	fi

check-deps:  ## Check for required development tools (Internal)
	@echo "Checking core dependencies (Internal)..."
	@$(call CHECK_DEP,$(QMAKE),qt6-base-dev)
	@$(call CHECK_DEP,g++,build-essential)
	@$(call CHECK_DEP,make,build-essential)

check-lint:
	@$(call CHECK_DEP,clang-format,clang-format)

 check-trunk:
	@$(call WARN_DEP,trunk)

check-docs:
	@$(call CHECK_DEP,doxygen,doxygen)

lint:  ## Run code linting (runs inside Docker)
	$(call IN_DOCKER_WRAPPER,lint)

lint_internal: check-lint check-trunk ## Internal lint target
	@echo "Running lint check..."
	@if command -v trunk >/dev/null 2>&1; then \
		echo "Running trunk check..."; \
		trunk check -a -y || true; \
	fi
	find src -name "*.h" -o -name "*.cpp" | xargs clang-format -n --verbose -style=file

format:  ## Run code formatting (runs inside Docker)
	$(call IN_DOCKER_WRAPPER,format)

format_internal: check-lint check-trunk ## Internal format target
	@echo "Formatting code..."
	@if command -v trunk >/dev/null 2>&1; then \
		echo "Running trunk fmt..."; \
		trunk fmt -a || true; \
	fi
	find src -name "*.h" -o -name "*.cpp" | xargs clang-format -i -verbose -style=file

docs:  ## Generate documentation (runs inside Docker)
	$(call IN_DOCKER_WRAPPER,docs)

docs_internal: check-docs ## Internal docs target
	@echo "Generating documentation..."
	@if [ -f Doxyfile ]; then \
		doxygen Doxyfile; \
	else \
		echo "Warning: Doxyfile not found. Skipping docs generation."; \
	fi

# ============================================================================
# Testing
# ============================================================================

test:  ## Run tests (runs inside Docker)
	$(call IN_DOCKER_WRAPPER,test)

test_internal: ## Internal test target
	@echo "Running examples..."
	./examples/showbox_demo.sh || true

test-icons:  ## Test icons demo (runs inside Docker)
	$(call IN_DOCKER_WRAPPER,test-icons)

test-icons_internal: ## Internal test-icons target
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
	@echo "===================================================================="
	@echo "Usage: make [target]"
	@echo ""
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[36m%-20s\033[0m %s\n", $$1, $$2}'
