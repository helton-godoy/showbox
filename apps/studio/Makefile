# ShowBox Studio Makefile
# IDE for ShowBox - Build and Development Automation

.PHONY: all build clean install run test help
.PHONY: dev start-dev start-dev-build
.PHONY: lint format docs check-deps
.PHONY: pkg-appimage

# Variables
CMAKE := cmake
MAKE := make
NPROC := $(shell nproc)
BUILD_DIR := .
BIN_DIR := build
TARGET_BIN := showbox-studio

# Default target
all: docker-check-deps build

# ============================================================================
# Docker Integration
# ============================================================================

DOCKER_SCRIPT := ../showbox/src/start-dev.sh
DOCKER_IMAGE := showbox-dev:latest

# Check if we are running inside docker
IN_DOCKER := $(shell [ -f /.dockerenv ] && echo 1 || echo 0)

# Wrapper to run make inside Docker
# Usage: $(call IN_DOCKER_WRAPPER, target_name)
define IN_DOCKER_WRAPPER
	@if [ "$(IN_DOCKER)" = "0" ]; then \
		echo "Detected host environment. Switching to Docker container..."; \
		$(DOCKER_SCRIPT) make $(1); \
	else \
		$(MAKE) $(1)_internal; \
	fi
endef

docker-shell:  ## Open a shell in the Docker container
	$(DOCKER_SCRIPT)

# ============================================================================
# Development
# ============================================================================

build:  ## Compile ShowBox Studio (runs inside Docker)
	$(call IN_DOCKER_WRAPPER,build)

build_internal: check-deps ## Internal build target
	@echo "Building ShowBox Studio (Inside Docker)..."
	mkdir -p $(BIN_DIR)
	cd $(BIN_DIR) && $(CMAKE) .. -DSHOWBOX_ROOT=../showbox && $(MAKE) -j$(NPROC)

clean:  ## Clean build artifacts (runs inside Docker)
	$(call IN_DOCKER_WRAPPER,clean)

clean_internal: ## Internal clean target
	@echo "Cleaning build artifacts..."
	rm -rf $(BIN_DIR)
	find . -name "*.o" -delete
	find . -name "moc_*" -delete

install: build  ## Install Studio (Desktop File)
	$(call IN_DOCKER_WRAPPER,install)

install_internal: build_internal
	@echo "Installing ShowBox Studio..."
	# Logic to install desktop file or copy binary

run: build  ## Run ShowBox Studio
	$(call IN_DOCKER_WRAPPER,run)

run_internal: build_internal
	@echo "Running ShowBox Studio..."
	./$(BIN_DIR)/$(TARGET_BIN)

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
	@$(call CHECK_DEP,cmake,cmake)
	@$(call CHECK_DEP,$(MAKE),build-essential)
	@$(call CHECK_DEP,g++,build-essential)

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
	@echo "Running tests..."
	cd $(BIN_DIR) && ctest || echo "No tests configured or build dir missing."

# ============================================================================
# Packaging
# ============================================================================

pkg-appimage:  ## Build AppImage
	@echo "Building AppImage..."
	./packaging/start-pkg-appimage.sh

# ============================================================================
# Help
# ============================================================================

help:  ## Show this help
	@echo "ShowBox Studio Makefile"
	@echo "===================================================================="
	@echo "Usage: make [target]"
	@echo ""
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[36m%-20s\033[0m %s\n", $$1, $$2}'
