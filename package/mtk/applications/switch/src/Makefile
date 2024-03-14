EXEC = switch
LEGACY_SWITCH = n

ifeq ($(CONFIG_P5_RGMII_TO_MT7530_MODE),y)
EXEC += switch_7530 
endif

ifeq ($(CONFIG_MACH_LEOPARD),y)
EXEC += switch_rtk switch_gsw
endif

CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)

ifeq ($(CONFIG_DEFAULTS_KERNEL_4_4),y)
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/ethernet/raeth
CFLAGS += -I$(ROOTDIR)/kernel_headers/include
else
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth -Werror
CFLAGS += -I$(ROOTDIR)/kernel_headers/include
endif

ifeq ($(CONFIG_RALINK_MT7620),y)
SRC_OLD=switch_gsw.c
ifeq ($(CONFIG_P5_RGMII_TO_MT7530_MODE),y)
SRC=switch_fun.c switch_753x.c switch_ioctl.c
endif
else
ifeq ($(CONFIG_RALINK_MT7621),y)
SRC=switch_fun.c switch_753x.c switch_ioctl.c
else
ifeq ($(CONFIG_SOC_MT7621),y)
SRC=switch_fun.c switch_753x.c switch_ioctl.c
else
ifeq ($(CONFIG_MACH_MT7623),y)
SRC=switch_fun.c switch_753x.c switch_ioctl.c
else
ifneq ($(CONFIG_MACH_LEOPARD)$(CONFIG_MT753X_GSW),)
SRC_RTK=switch_rtk.c
SRC_GSW=switch.c
SRC=switch_fun.c switch_753x.c switch_ioctl.c
else
LEGACY_SWITCH=y
ifeq ($(CONFIG_GE1_SGMII_FORCE_2500),y)
SRC=switch_rtk.c
else
SRC=switch.c
endif
endif
endif
endif
endif
endif

ifneq ($(CONFIG_SUPPORT_OPENWRT), y)
CFLAGS += -DCONFIG_RALINK_MT7620
CONF_H  = $(ROOTDIR)/$(LINUXDIR)/include/linux/autoconf.h
else
ifeq ($(LEGACY_SWITCH), n)
SRC += switch_netlink.c
endif
LIBS += -lnl -lnl-genl
endif


all: $(EXEC)

ifeq ($(CONFIG_RALINK_MT7620),y)
switch: $(SRC_OLD) $(CONF_H)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRC_OLD) $(LDLIBS)
ifeq ($(CONFIG_P5_RGMII_TO_MT7530_MODE),y)
switch_7530: $(SRC) $(CONF_H)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRC) $(LDLIBS) $(LIBS)
endif
else
switch: $(SRC) $(CONF_H)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRC) $(LDLIBS) $(LIBS)
endif

ifeq ($(CONFIG_MACH_LEOPARD),y)
switch_rtk: $(SRC_RTK) $(CONF_H)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRC_RTK) $(LDLIBS)
switch_gsw: $(SRC_GSW) $(CONF_H)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRC_GSW) $(LDLIBS)
endif


romfs:
	$(ROMFSINST) /bin/switch
ifeq ($(CONFIG_MACH_LEOPARD),y)
	$(ROMFSINST) /bin/switch_rtk
	$(ROMFSINST) /bin/switch_gsw
endif
ifeq ($(CONFIG_P5_RGMII_TO_MT7530_MODE),y)
	$(ROMFSINST) /bin/switch_7530
endif

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
