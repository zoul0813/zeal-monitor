BIN=mon.bin

ifndef ZOS_PATH
    $(error "Failure: ZOS_PATH variable not found. It must point to Zeal 8-bit OS path.")
endif

include $(ZOS_PATH)/kernel_headers/sdcc/base_sdcc.mk

run:
	$(ZEAL_NATIVE_BIN) -H bin -r $(ZEAL_NATIVE_ROM) #-t tf.img -e eeprom.img

native: all run