BOARD ?= xiao_ble//zmk
SHIELD ?= clicktaptoe
ARTIFACT_NAME ?= clicktaptoe_xiao_ble_zmk

ZMK_APP ?= ../zmk/app
CONFIG_DIR ?= $(CURDIR)/config
MODULE_DIR ?= $(CURDIR)
BUILD_DIR ?= build/$(SHIELD)
UF2_DIR ?= $(CURDIR)/firmware
UF2_FILE ?= $(UF2_DIR)/$(ARTIFACT_NAME).uf2

.PHONY: help build pristine uf2 clean

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

build:
	cd "$(ZMK_APP)" && west build -d "$(BUILD_DIR)" -b "$(BOARD)" -- \
		-DSHIELD="$(SHIELD)" \
		-DZMK_CONFIG="$(CONFIG_DIR)" \
		-DZMK_EXTRA_MODULES="$(MODULE_DIR)"

pristine:
	cd "$(ZMK_APP)" && west build -p always -d "$(BUILD_DIR)" -b "$(BOARD)" -- \
		-DSHIELD="$(SHIELD)" \
		-DZMK_CONFIG="$(CONFIG_DIR)" \
		-DZMK_EXTRA_MODULES="$(MODULE_DIR)"

uf2: build
	mkdir -p "$(UF2_DIR)"
	cp "$(ZMK_APP)/$(BUILD_DIR)/zephyr/zmk.uf2" "$(UF2_FILE)"
	@echo "UF2 copied to $(UF2_FILE)"

clean:
	rm -rf "$(ZMK_APP)/$(BUILD_DIR)" "$(UF2_DIR)"
