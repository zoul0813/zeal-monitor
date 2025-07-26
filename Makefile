BIN=mon.bin

ifndef ZOS_PATH
    $(error "Failure: ZOS_PATH variable not found. It must point to Zeal 8-bit OS path.")
endif

include $(ZOS_PATH)/kernel_headers/sdcc/base_sdcc.mk

ZOS_ASM_INCLUDE = $(ZOS_PATH)/kernel_headers/z88dk-z80asm/

all::
	z88dk-z80asm -b -I$(ZOS_ASM_INCLUDE) -O$(OUTPUT_DIR) -oa.out src/example.asm

run:
	$(ZEAL_NATIVE_BIN) -H bin -r $(ZEAL_NATIVE_ROM) #-t tf.img -e eeprom.img

native: all run