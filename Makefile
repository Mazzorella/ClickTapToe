BOARD ?= xiao_ble//zmk
SHIELD ?= clicktaptoe
ARTIFACT_NAME ?= clicktaptoe_xiao_usb_zmk

ZMK_APP ?= ../zmk/app
CONFIG_DIR ?= $(CURDIR)/config
MODULE_DIR ?= $(CURDIR)
PMW3610_MODULE ?= $(abspath $(CURDIR)/../zmk-pmw3610-driver)
EXTRA_MODULES ?= $(MODULE_DIR);$(PMW3610_MODULE)
BUILD_DIR ?= $(CURDIR)/build/$(SHIELD)
UF2_DIR ?= $(CURDIR)/firmware
UF2_FILE ?= $(UF2_DIR)/$(ARTIFACT_NAME).uf2

.PHONY: help check-pmw3610-module build pristine uf2 clean

help:
	@echo "ClickTapToe ZMK build targets:"
	@echo "  make build     Build firmware with west"
	@echo "  make pristine  Force a clean west rebuild"
	@echo "  make uf2       Build and copy zmk.uf2 to firmware/"
	@echo "  make clean     Remove local build and copied firmware output"
	@echo
	@echo "Defaults:"
	@echo "  ZMK_APP=$(ZMK_APP)"
	@echo "  BOARD=$(BOARD)"
	@echo "  SHIELD=$(SHIELD)"
	@echo "  BUILD_DIR=$(BUILD_DIR)"
	@echo "  PMW3610_MODULE=$(PMW3610_MODULE)"

check-pmw3610-module:
	@test -f "$(PMW3610_MODULE)/zephyr/module.yml" || \
		( echo "PMW3610 driver module not found at $(PMW3610_MODULE)"; \
		  echo "Clone https://github.com/badjeff/zmk-pmw3610-driver there or override PMW3610_MODULE."; \
		  exit 1 )

build: check-pmw3610-module
	cd "$(ZMK_APP)" && west build -d "$(BUILD_DIR)" -b "$(BOARD)" -- \
		-DSHIELD="$(SHIELD)" \
		-DZMK_CONFIG="$(CONFIG_DIR)" \
		-DZMK_EXTRA_MODULES="$(EXTRA_MODULES)"

pristine: check-pmw3610-module
	cd "$(ZMK_APP)" && west build -p always -d "$(BUILD_DIR)" -b "$(BOARD)" -- \
		-DSHIELD="$(SHIELD)" \
		-DZMK_CONFIG="$(CONFIG_DIR)" \
		-DZMK_EXTRA_MODULES="$(EXTRA_MODULES)"

uf2: build
	mkdir -p "$(UF2_DIR)"
	cp "$(BUILD_DIR)/zephyr/zmk.uf2" "$(UF2_FILE)"
	@echo "UF2 copied to $(UF2_FILE)"

clean:
	rm -rf "$(BUILD_DIR)" "$(UF2_DIR)"
