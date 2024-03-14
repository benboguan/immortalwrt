ccflags-y += $(MODULE_FLAGS)

MODULE_NAME=mtk_warp_proxy

CHIP_DIR=chips

OBJS:= warp_proxy.o
OBJS += $(CHIP_DIR)/warp_wifi_mt7615.o
#######################################
# For integrating to kernel source
######################################
obj-m += $(MODULE_NAME).o


$(MODULE_NAME)-objs += $(OBJS)

install:

clean:
	rm -rf *.o *.ko *.modules .*.cmd .*.o *.order $(MODULE_NAME).mod.c
