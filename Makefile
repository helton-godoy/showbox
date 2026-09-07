BUILD_DIR ?= build
CMAKE ?= cmake

.PHONY: all build configure test clean install pkg-deb pkg-rpm pkg-appimage pkg-install-smoke help

all: build

configure:
	$(CMAKE) -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release

build: configure
	$(CMAKE) --build $(BUILD_DIR) --parallel

test: build
	QT_QPA_PLATFORM=offscreen ctest --test-dir $(BUILD_DIR) --output-on-failure

clean:
	$(CMAKE) -E remove_directory $(BUILD_DIR)

install: build
	$(CMAKE) --install $(BUILD_DIR)

pkg-deb:
	./packaging/deb/start-pkg-deb.sh all

pkg-rpm:
	./packaging/rpm/start-pkg-rpm.sh

pkg-appimage:
	./packaging/appimage/start-pkg-appimage.sh

pkg-install-smoke:
	./tests/installation/install_smoke.sh $(ARGS)

help:
	@echo "Targets: configure build test clean install pkg-deb pkg-rpm pkg-appimage pkg-install-smoke"
