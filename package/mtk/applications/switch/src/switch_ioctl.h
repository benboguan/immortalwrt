/*
 * switch_ioctl.h: switch(ioctl) set API
 */

#ifndef SWITCH_IOCTL_H
#define SWITCH_IOCTL_H

#ifndef CONFIG_SUPPORT_OPENWRT
#include <linux/autoconf.h>
#define ETH_DEVNAME "eth2"
#define BR_DEVNAME "br0"
#else
#define ETH_DEVNAME "eth0"
#define BR_DEVNAME "br-lan"
#endif

extern int chip_name;

void switch_ioctl_init(void);
void switch_ioctl_fini(void);
int reg_read_ioctl(int offset, int *value);
int reg_write_ioctl(int offset, int value);
int phy_dump_ioctl(int phy_addr);
int mii_mgr_cl22_read_ioctl(unsigned int port_num, unsigned int reg, int *value);
int mii_mgr_cl22_write_ioctl(unsigned int port_num, unsigned int reg,
			unsigned int value);
int mii_mgr_cl45_read_ioctl(int port_num, int dev, int reg, int *value);
int mii_mgr_cl45_write_ioctl(int port_num, int dev, int reg, int value);
#endif
