#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>

#ifndef CONFIG_SUPPORT_OPENWRT
#include <linux/autoconf.h>
#endif

#include "ra_ioctl.h"


#if defined (CONFIG_RALINK_MT7620)
#define MAX_PORT		7
#else
#define MAX_PORT		6
#endif

#undef IFR_NAME
#if defined (CONFIG_SUPPORT_OPENWRT)
#define IFR_NAME "eth0"
#else
#define IFR_NAME "eth2"
#endif

int esw_fd;

void switch_init(void)
{
	esw_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (esw_fd < 0) {
		perror("socket");
		exit(0);
	}
}

void switch_fini(void)
{
	close(esw_fd);
}

void usage(char *cmd)
{
	printf("Usage:\n");
	printf(" %s mib                                    - dump mib counter\n", cmd);
	printf(" %s dump                                   - dump switch table\n", cmd);
	printf(" %s clear                                  - clear switch table\n", cmd);
	printf(" %s add [mac] [port] [vlan id]             - add a static entry to switch table\n", cmd);
	printf(" %s del [mac] [port] [vlan id]             - add an entry from switch table\n", cmd);
	printf(" %s ingress-rate on [port] [Kbps]          - set ingress rate limit on port 0~4 \n", cmd);
	printf(" %s egress-rate on [port] [Kbps]           - set egress rate limit on port 0~4 \n", cmd);
	printf(" %s ingress-rate off [port]                - del ingress rate limit on port 0~4 \n", cmd);
	printf(" %s egress-rate off [port]                 - del egress rate limit on port 0~4\n", cmd);
	printf(" %s igmpsnoop on                           - enable hw igmp snoop\n", cmd);
	printf(" %s igmpsnoop off                          - disable hw igmp snoop\n", cmd);
	printf(" %s mirror [monitor_port] [target_rx_portmask] [target_tx_portmask]    - set port mirror\n", cmd);
	printf(" %s phy [phy_addr]			               - get phy link status\n", cmd);
	printf(" %s regs r [offset]                        - register read from offset\n", cmd);
	printf(" %s regs w [offset] [value]                - register write value to offset\n", cmd);
	printf(" %s vlan dump                              - dump switch vlan setting\n", cmd);
	printf("                                               - portmap is the order of port 0~4, port16, port17.\n");
	printf(" %s vlan clear                             - clear switch vlan setting\n", cmd);
	printf(" %s vlan set [vlan idx] [vid] [portmap]    - set vlan id and associated member.\n", cmd);
	printf("                                               - portmap is the order of port 0~4, port16, port17.\n");
	printf(" %s tag on [port] [vid]                    - keep vlan tag for egress packet on prot 0~4, 16, 17\n", cmd);
	printf(" %s tag off [port] [vid]                   - remove vlan tag for egress packet on port 0~4, 16, 17\n", cmd);
	printf(" %s test_mode [port] [mode]                - set phy test mode. port: 0~4; mode: 1 or 4\n", cmd);
	printf(" %s port_trunk [portmap]                   - set port trunk. portmap is in the order of port 0~3\n", cmd);
	printf(" %s qos on                                 - enable switch qos\n", cmd);
	printf(" %s qos off                                - disable switch qos\n", cmd);
	printf(" %s qos set_table2type [table] [type]      - set table qos type\n", cmd);
	printf(" %s qos get_table2type [table]             - get table qos type\n", cmd);
	printf(" %s qos set_port2table [port] [table]      - set port to table mapping\n", cmd);
	printf(" %s qos get_port2table [port]              - get port to table mapping\n", cmd);
	printf(" %s qos set_port2pri [port] [pri]          - set port to priority mapping\n", cmd);
	printf(" %s qos get_port2pri [port]                - get port to priority mapping\n", cmd);
	printf(" %s qos set_dscp2pri [dscp] [pri]          - set dscp to priority mapping\n", cmd);
	printf(" %s qos get_dscp2pri [dscp]                - get dscp to priority mapping\n", cmd);
	printf(" %s qos set_pri2queue [pri] [queue]        - set priority to queue mapping\n", cmd);
	printf(" %s qos get_pri2queue                      - get priority to queue mapping\n", cmd);
	printf(" %s qos set_weight [port] [queue] [weight] - set weight\n", cmd);
	printf(" %s qos get_weight [port]                  - get weights of queues\n", cmd);
	printf(" table: 0, 1\n");
	printf(" type: port =0; 1q = 1; acl = 2; dscp = 3; cvlan = 4; svlan = 5; dmac = 6; smac = 7\n");
	printf(" port: LAN/WAN ports: 0 ~ 4; CPU ports: 16, 17\n");
	printf(" dscp: 0 ~ 63\n");
	printf(" pri: 0 ~ 7\n");
	printf(" queue: 0 ~ 7\n");
	printf(" weight: 0 for SPQ; 1 ~ 127 for WFQ\n");
	switch_fini();
	exit(0);
}

int reg_read(int offset, int *value)
{
	struct ifreq ifr;
	struct ra_switch_ioctl_data data;

	if (value == NULL)
		return -1;
	data.reg_addr = offset;
	data.cmd = SW_IOCTL_READ_REG;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
	ifr.ifr_data = &data;
	if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
		perror("ioctl");
		close(esw_fd);
		exit(0);
	}
	return 0;
}

int reg_write(int offset, int value)
{
	struct ifreq ifr;
	struct ra_switch_ioctl_data data;

	data.reg_addr = offset;
	data.reg_val = value;
	data.cmd = SW_IOCTL_WRITE_REG;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
	ifr.ifr_data = &data;
	if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
		perror("ioctl");
		close(esw_fd);
		exit(0);
	}
	return 0;
}

int phy_dump(int phy_addr)
{
	struct ifreq ifr;
    struct ra_switch_ioctl_data data;

	data.port = phy_addr;
	data.cmd = SW_IOCTL_GET_PHY_STATUS;
	ifr.ifr_data = &data;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
	if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
		perror("ioctl");
		close(esw_fd);
		exit(0);
	}
	return 0;
}

int ingress_rate_set(int on_off, int port, int bw)
{
	struct ifreq ifr;
	struct ra_switch_ioctl_data data;

    data.on_off = on_off;
    data.port = port;
	data.bw = bw;
	data.cmd = SW_IOCTL_SET_INGRESS_RATE;
	ifr.ifr_data = &data;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
	if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
		perror("ioctl");
		close(esw_fd);
		exit(0);
	}

	return 0;
}

int egress_rate_set(int on_off, int port, int bw)
{
	struct ifreq ifr;
	struct ra_switch_ioctl_data data;

    data.on_off = on_off;
    data.port = port;
	data.bw = bw;
	data.cmd = SW_IOCTL_SET_EGRESS_RATE;
	ifr.ifr_data = &data;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
	if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
		perror("ioctl");
		close(esw_fd);
		exit(0);
	}
	return 0;
}

int
getnext (
	char *	src,
	int	separator,
	char *	dest
	)
{
    char *	c;
    int	len;

    if ( (src == NULL) || (dest == NULL) ) {
	return -1;
    }

    c = strchr(src, separator);
    if (c == NULL) {
	strcpy(dest, src);
	return -1;
    }
    len = c - src;
    strncpy(dest, src, len);
    dest[len] = '\0';
    return len + 1;
}

int
str_to_ip (
	unsigned int *	ip,
	char *		str
	)
{
    int		len;
    char *		ptr = str;
    char		buf[128];
    unsigned char	c[4];
    int		i;

    for (i = 0; i < 3; ++i) {
	if ((len = getnext(ptr, '.', buf)) == -1) {
	    return 1; /* parse error */
	}
	c[i] = atoi(buf);
	ptr += len;
    }
    c[3] = atoi(ptr);
    *ip = (c[0]<<24) + (c[1]<<16) + (c[2]<<8) + c[3];
    return 0;
}

/* convert IP address from number to string */
void
ip_to_str (
	char *		str,
	unsigned int	ip
	)
{
    unsigned char *	ptr = (char *)&ip;
    unsigned char	c[4];

    c[0] = *(ptr);
    c[1] = *(ptr+1);
    c[2] = *(ptr+2);
    c[3] = *(ptr+3);
    /* sprintf(str, "%d.%d.%d.%d", c[0], c[1], c[2], c[3]); */
    sprintf(str, "%d.%d.%d.%d", c[3], c[2], c[1], c[0]);
}

void table_dump(void)
{
    struct ifreq ifr;
    struct ra_switch_ioctl_data data;

    data.cmd = SW_IOCTL_DUMP_TABLE;
    ifr.ifr_data = &data;
    strncpy(ifr.ifr_name, IFR_NAME, 5);
    if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
}

void table_clear(void)
{
    struct ifreq ifr;
    struct ra_switch_ioctl_data data;

    data.cmd = SW_IOCTL_CLEAR_TABLE;
    ifr.ifr_data = &data;
    strncpy(ifr.ifr_name, IFR_NAME, 5);
    if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
}

void table_add(int argc, char *argv[])
{
	struct ra_switch_ioctl_data data;
    struct ifreq ifr;
	int i;
	char tmpstr[9];
	unsigned int value;

	strncpy(tmpstr, argv[2], 8);
	tmpstr[12] = '\0';
	value = strtoul(tmpstr, NULL, 16);
	for (i = 0; i < 4; i ++) {
		data.mac[3 - i] = (value >> (i*8)) & 0xff;
	}

	strncpy(tmpstr, argv[2] + 8, 4);
	tmpstr[12] = '\0';
	value = strtoul(tmpstr, NULL, 16);
	for (i = 0; i < 2; i ++) {
		data.mac[5 - i] = (value >> ((i+2)*8)) & 0xff;
	}

	data.port = strtoul(argv[3], NULL, 0);
	data.vid = strtoul(argv[4], NULL, 0);
	data.cmd = SW_IOCTL_ADD_L2_ADDR;
	ifr.ifr_data = &data;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
	if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
}

void table_del(int argc, char *argv[])
{
	struct ra_switch_ioctl_data data;
    struct ifreq ifr;
	int i;
	char tmpstr[9];
	unsigned int value;

	strncpy(tmpstr, argv[2], 8);
	tmpstr[12] = '\0';
	value = strtoul(tmpstr, NULL, 16);
	for (i = 0; i < 4; i ++) {
		data.mac[3 - i] = (value >> (i*8)) & 0xff;
	}

	strncpy(tmpstr, argv[2] + 8, 4);
	tmpstr[12] = '\0';
	value = strtoul(tmpstr, NULL, 16);
	for (i = 0; i < 2; i ++) {
		data.mac[5 - i] = (value >> ((i+2)*8)) & 0xff;
	}

	data.port = strtoul(argv[3], NULL, 0);
	data.vid = strtoul(argv[4], NULL, 0);
	data.cmd = SW_IOCTL_DEL_L2_ADDR;
	ifr.ifr_data = &data;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
	if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
}

void set_port_mirror(int argc, char *argv[])
{
	struct ra_switch_ioctl_data data;
    struct ifreq ifr;
	int i;

	if (strlen(argv[3]) != 7) {
        printf("rx portmap format error, should be of length 7\n");
        return;
    }

	if (strlen(argv[4]) != 7) {
        printf("tx portmap format error, should be of length 7\n");
        return;
    }

	data.port = strtoul(argv[2], NULL, 0);

    data.rx_port_map= 0;
    for (i = 0; i < 7; i++) {
        if (argv[3][i] != '0' && argv[3][i] != '1') {
            printf("rx portmap format error, should be of combination of 0 or 1\n");
            return;
        }
        data.rx_port_map += (argv[3][i] - '0') * (1 << i);
    }

	data.tx_port_map= 0;
    for (i = 0; i < 7; i++) {
        if (argv[4][i] != '0' && argv[4][i] != '1') {
            printf("tx portmap format error, should be of combination of 0 or 1\n");
            return;
        }
        data.tx_port_map += (argv[4][i] - '0') * (1 << i);
    }
	data.cmd = SW_IOCTL_SET_PORT_MIRROR;
    ifr.ifr_data = &data;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
    if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
}

void vlan_dump(void)
{
	struct ifreq ifr;
	struct ra_switch_ioctl_data data;

	data.cmd = SW_IOCTL_DUMP_VLAN;
	ifr.ifr_data = &data;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
	if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
		perror("ioctl");
		close(esw_fd);
		exit(0);
	}
}

void vlan_clear(void)
{
	struct ifreq ifr;
	struct ra_switch_ioctl_data data;

	data.cmd = SW_IOCTL_CLEAR_VLAN;
	ifr.ifr_data = &data;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
	if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
		perror("ioctl");
		close(esw_fd);
		exit(0);
	}
}

void vlan_set(int argc, char *argv[])
{
	unsigned int i;
	struct ra_switch_ioctl_data data;
	struct ifreq ifr;


	if (argc < 5) {
		printf("insufficient arguments!\n");
		return;
	}
	data.vid= strtoul(argv[4], NULL, 0);
	if (data.vid < 0 || 0xfff < data.vid) {
		printf("wrong vlan id range, should be within 0~4095\n");
		return;
	}
	if (strlen(argv[5]) != 7) {
		printf("portmap format error, should be of length 7\n");
		return;
	}
	data.port_map= 0;
	for (i = 0; i < 7; i++) {
		if (argv[5][i] != '0' && argv[5][i] != '1') {
			printf("portmap format error, should be of combination of 0 or 1\n");
			return;
		}
		data.port_map += (argv[5][i] - '0') * (1 << i);
	}
	data.cmd = SW_IOCTL_SET_VLAN;
	ifr.ifr_data = &data;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
	if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
}

void vlan_tag(int argc, char *argv[])
{
	struct ra_switch_ioctl_data data;
	struct ifreq ifr;

	if (!strncmp(argv[2], "on", 3))
		data.on_off = 1;
	else
		data.on_off = 0;
	data.port = strtoul(argv[3], NULL, 10);
	data.vid = strtoul(argv[4], NULL, 10);
	data.cmd = SW_IOCTL_VLAN_TAG;
	ifr.ifr_data = &data;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
	if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
}

void vlan_mode(int argc, char *argv[])
{
	struct ra_switch_ioctl_data data;
	struct ifreq ifr;

	data.mode = strtoul(argv[3], NULL, 10);
	data.cmd = SW_IOCTL_SET_VLAN_MODE;
	ifr.ifr_data = &data;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
	if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
}

void igmp_on(int argc, char *argv[])
{
    struct ifreq ifr;
    struct ra_switch_ioctl_data data;

    data.cmd = SW_IOCTL_ENABLE_IGMPSNOOP;
	if (argc == 4)
		data.on_off = strtoul(argv[3], NULL, 10);
	else
		data.on_off = 0;
    ifr.ifr_data = &data;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
    if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
}

void igmp_off()
{
	struct ifreq ifr;
    struct ra_switch_ioctl_data data;

    data.cmd = SW_IOCTL_DISABLE_IGMPSNOOP;
    ifr.ifr_data = &data;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
    if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
}

void dump_mib(void)
{
	struct ifreq ifr;
	struct ra_switch_ioctl_data data;

	data.cmd = SW_IOCTL_DUMP_MIB;
	ifr.ifr_data = &data;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
	if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
		perror("ioctl");
		close(esw_fd);
		exit(0);
	}
}

void qos_on(void)
{
    struct ifreq ifr;
    struct ra_switch_ioctl_data data;

    data.cmd = SW_IOCTL_QOS_EN;
	data.on_off = 1;
    ifr.ifr_data = &data;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
    if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
}

void qos_off(void)
{
    struct ifreq ifr;
    struct ra_switch_ioctl_data data;

    data.cmd = SW_IOCTL_QOS_EN;
	data.on_off = 0;
    ifr.ifr_data = &data;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
    if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
}

void qos_set_table2type(int argc, char *argv[])
{
    struct ifreq ifr;
    struct ra_switch_ioctl_data data;

    data.cmd = SW_IOCTL_QOS_SET_TABLE2TYPE;
	data.qos_table_idx = strtoul(argv[3], NULL, 10);
	data.qos_type = strtoul(argv[4], NULL, 10);
    ifr.ifr_data = &data;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
    if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
}

void qos_get_table2type(int argc, char *argv[])
{
    struct ifreq ifr;
    struct ra_switch_ioctl_data data;

    data.cmd = SW_IOCTL_QOS_GET_TABLE2TYPE;
	data.qos_table_idx = strtoul(argv[3], NULL, 10);
    ifr.ifr_data = &data;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
    if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
}

void qos_set_port2table(int argc, char *argv[])
{
    struct ifreq ifr;
    struct ra_switch_ioctl_data data;

    data.cmd = SW_IOCTL_QOS_SET_PORT2TABLE;
    data.port = strtoul(argv[3], NULL, 10);
    data.qos_table_idx = strtoul(argv[4], NULL, 10);
    ifr.ifr_data = &data;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
    if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
}

void qos_get_port2table(int argc, char *argv[])
{
    struct ifreq ifr;
    struct ra_switch_ioctl_data data;

    data.cmd = SW_IOCTL_QOS_GET_PORT2TABLE;
    data.port = strtoul(argv[3], NULL, 10);
    ifr.ifr_data = &data;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
    if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
}

void qos_set_port2pri(int argc, char *argv[])
{
	struct ifreq ifr;
    struct ra_switch_ioctl_data data;

	data.cmd = SW_IOCTL_QOS_SET_PORT2PRI;
	data.port = strtoul(argv[3], NULL, 10);
	data.qos_pri = strtoul(argv[4], NULL, 10);
	ifr.ifr_data = &data;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
    if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
}

void qos_get_port2pri(int argc, char *argv[])
{
	struct ifreq ifr;
    struct ra_switch_ioctl_data data;

	data.cmd = SW_IOCTL_QOS_GET_PORT2PRI;
	data.port = strtoul(argv[3], NULL, 10);
	ifr.ifr_data = &data;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
    if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
}

void qos_set_dscp2pri(int argc, char *argv[])
{
	struct ifreq ifr;
    struct ra_switch_ioctl_data data;

	data.cmd = SW_IOCTL_QOS_SET_DSCP2PRI;
	data.qos_dscp = strtoul(argv[3], NULL, 10);
	data.qos_pri = strtoul(argv[4], NULL, 10);
	ifr.ifr_data = &data;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
    if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
}

void qos_get_dscp2pri(int argc, char *argv[])
{
	struct ifreq ifr;
    struct ra_switch_ioctl_data data;

	data.cmd = SW_IOCTL_QOS_GET_DSCP2PRI;
	data.qos_dscp = strtoul(argv[3], NULL, 10);
	ifr.ifr_data = &data;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
    if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
}

void qos_set_pri2queue(int argc, char *argv[])
{
    struct ifreq ifr;
    struct ra_switch_ioctl_data data;

    data.cmd = SW_IOCTL_QOS_SET_PRI2QUEUE;
    data.qos_pri = strtoul(argv[3], NULL, 10);
    data.qos_queue_num = strtoul(argv[4], NULL, 10);
    ifr.ifr_data = &data;
	if (data.qos_queue_num < 8) {
	strncpy(ifr.ifr_name, IFR_NAME, 5);
    	if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
        	perror("ioctl");
        	close(esw_fd);
	        exit(0);
    	}
	}	else
			printf("queue should be 0 ~ 7\n");

}

void qos_get_pri2queue(int argc, char *argv[])
{
    struct ifreq ifr;
    struct ra_switch_ioctl_data data;

    data.cmd = SW_IOCTL_QOS_GET_PRI2QUEUE;
    ifr.ifr_data = &data;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
    if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
}

void qos_set_weight(int argc, char *argv[])
{
	struct ifreq ifr;
    struct ra_switch_ioctl_data data;

	data.cmd = SW_IOCTL_QOS_SET_QUEUE_WEIGHT;
	data.port = strtoul(argv[3], NULL, 10);
    data.qos_queue_num = strtoul(argv[4], NULL, 10);
	if (data.qos_queue_num < 8) {
	    data.qos_weight = strtoul(argv[5], NULL, 10);
		ifr.ifr_data = &data;
		strncpy(ifr.ifr_name, IFR_NAME, 5);
    	if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
        	perror("ioctl");
        	close(esw_fd);
        	exit(0);
    	}
	} else
		printf("queue should be 0 ~ 7\n");
}

void qos_get_weight(int argc, char *argv[])
{
	struct ifreq ifr;
    struct ra_switch_ioctl_data data;

	data.cmd = SW_IOCTL_QOS_GET_QUEUE_WEIGHT;
	data.port = strtoul(argv[3], NULL, 10);
	ifr.ifr_data = &data;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
    if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
}

void phy_test_mode(int argc, char *argv[])
{
	struct ifreq ifr;
    struct ra_switch_ioctl_data data;

	data.cmd = SW_IOCTL_SET_PHY_TEST_MODE;
	data.port = strtoul(argv[2], NULL, 10);
	data.mode = strtoul(argv[3], NULL, 10);
	ifr.ifr_data = &data;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
    if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
}

void set_port_trunk(int argc, char *argv[])
{
	struct ifreq ifr;
    struct ra_switch_ioctl_data data;
	int i;

	data.cmd = SW_IOCTL_SET_PORT_TRUNK;
	data.port_map= 0;
	for (i = 0; i < 4; i++) {
		if (argv[2][i] != '0' && argv[2][i] != '1') {
			printf("portmap format error, should be of combination of 0 or 1\n");
			return;
		}
		data.port_map += (argv[2][i] - '0') * (1 << i);
	}
	ifr.ifr_data = &data;
	strncpy(ifr.ifr_name, IFR_NAME, 5);
    if (-1 == ioctl(esw_fd, RAETH_SW_IOCTL, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
}

int main(int argc, char *argv[])
{
	int i,j;
    switch_init();

	if (argc < 2)
		usage(argv[0]);
	if (argc == 2) {
		if (!strncmp(argv[1], "dump", 5))
			table_dump();
        else if (!strncmp(argv[1], "mib", 4))
			dump_mib();
        else if (!strncmp(argv[1], "clear", 6))
			table_clear();
		else
			usage(argv[0]);
	} else if (!strncmp(argv[1], "phy", 4)) {
		if (argc == 3) {
				int phy_addr = strtoul(argv[2], NULL, 10);
				phy_dump(phy_addr);
		}else {
			phy_dump(32); //dump all phy register
		}
	} else if (!strncmp(argv[1], "mirror", 7)) {
		if (argc < 5)
			usage(argv[0]);
        else
			set_port_mirror(argc, argv);
	} else if (!strncmp(argv[1], "vlan", 5)) {
		if (argc < 3)
			usage(argv[0]);
		if (!strncmp(argv[2], "dump", 5))
			vlan_dump();
		else if (!strncmp(argv[2], "set", 4))
			vlan_set(argc, argv);
		else if (!strncmp(argv[2], "clear", 6))
			vlan_clear();
		else if (!strncmp(argv[2], "mode", 5))
			vlan_mode(argc, argv);

		else
			usage(argv[0]);
	} else if (!strncmp(argv[1], "tag", 4)) {
		if (argc < 5)
			usage(argv[0]);
		else
			vlan_tag(argc, argv);
	} else if (!strncmp(argv[1], "regs", 5)) {
		int off, val=0;
		if (argc < 4)
			usage(argv[0]);
		if (argv[2][0] == 'r') {
			off = strtoul(argv[3], NULL, 16);
			reg_read(off, &val);
		}
		else if (argv[2][0] == 'w') {
			if (argc != 5)
				usage(argv[0]);
			off = strtoul(argv[3], NULL, 16);
			val = strtoul(argv[4], NULL, 16);
			reg_write(off, val);
		}
		else if (argv[2][0] == 'd') {
			off = strtoul(argv[3], NULL, 16);
			for(i=0; i<16; i++)
			{
				printf("0x%08x: ", off+0x10*i);
				for(j=0; j<4; j++){
					reg_read(off+i*0x10+j*0x4, &val);
					printf(" 0x%08x", val);
				}
				printf("\n");
			}

		}
		else
			usage(argv[0]);
	} else if (!strncmp(argv[1], "ingress-rate", 6)) {
		int port=0, bw=0;

		if (argv[2][1] == 'n') {
			port = strtoul(argv[3], NULL, 0);
			bw = strtoul(argv[4], NULL, 0);
			ingress_rate_set(1, port, bw);
			printf("switch port=%d, bw=%d\n", port, bw);
		}
		else if (argv[2][1] == 'f') {
			if (argc != 4)
				usage(argv[0]);
			port = strtoul(argv[3], NULL, 0);
			ingress_rate_set(0, port, bw);
			printf("switch port=%d ingress rate limit off\n", port);
		}
		else
			usage(argv[0]);
	} else if (!strncmp(argv[1], "egress-rate", 6)) {
		int port=0, bw=0;

		if (argv[2][1] == 'n') {
			port = strtoul(argv[3], NULL, 0);
			bw = strtoul(argv[4], NULL, 0);
			egress_rate_set(1, port, bw);
			printf("switch port=%d, bw=%d\n", port, bw);
		}
		else if (argv[2][1] == 'f') {
			if (argc != 4)
				usage(argv[0]);
			port = strtoul(argv[3], NULL, 0);
			egress_rate_set(0, port, bw);
			printf("switch port=%d egress rate limit off\n", port);
		}
		else
			usage(argv[0]);
	} else if (!strncmp(argv[1], "tag", 4)) {
		int offset=0, value=0, port=0;

		port = strtoul(argv[3], NULL, 0);
		offset = 0x2004 + port * 0x100;
		reg_read(offset, &value);
		if (argv[2][1] == 'n') {
			value |= 0x20000000;
			reg_write(offset, value);
			printf("tag vid at port %d\n", port);
		}
		else if (argv[2][1] == 'f') {
			value &= 0xc0ffffff;
			reg_write(offset, value);
			printf("untag vid at port %d\n", port);
		}
		else
			usage(argv[0]);
	} else if (!strncmp(argv[1], "pvid", 5)) {
		int offset=0, value=0, port=0, pvid=0;

		port = strtoul(argv[2], NULL, 0);
	        pvid = strtoul(argv[3], NULL, 0);
		offset = 0x2014 + port * 0x100;
		reg_read(offset, &value);
                value &= 0xfffff000;
		value |= pvid;
		reg_write(offset, value);
		printf("Set port %d pvid %d.\n", port, pvid);
	} else if (!strncmp(argv[1], "igmpsnoop", 10)) {
		if (argc < 3)
			usage(argv[0]);
		if (!strncmp(argv[2], "on", 3))
				igmp_on(argc, argv);
		else if (!strncmp(argv[2], "off", 4))
			igmp_off();
		else
			usage(argv[0]);
	} else if (!strncmp(argv[1], "qos", 4)) {
		if (argc < 3)
			usage(argv[0]);
		if (!strncmp(argv[2], "on", 3))
            qos_on();
        else if (!strncmp(argv[2], "off", 4))
            qos_off();
		else if (!strncmp(argv[2], "set_table2type", 15))
            qos_set_table2type(argc, argv);
		else if (!strncmp(argv[2], "get_table2type", 15))
            qos_get_table2type(argc, argv);
		else if (!strncmp(argv[2], "set_port2table", 15))
            qos_set_port2table(argc, argv);
		else if (!strncmp(argv[2], "get_port2table", 15))
            qos_get_port2table(argc, argv);
		else if (!strncmp(argv[2], "set_port2pri", 13))
			qos_set_port2pri(argc, argv);
		else if (!strncmp(argv[2], "get_port2pri", 13))
			qos_get_port2pri(argc, argv);
		else if (!strncmp(argv[2], "set_dscp2pri", 13))
			qos_set_dscp2pri(argc, argv);
		else if (!strncmp(argv[2], "get_dscp2pri", 13))
			qos_get_dscp2pri(argc, argv);
		else if (!strncmp(argv[2], "set_pri2queue", 14))
			qos_set_pri2queue(argc, argv);
		else if (!strncmp(argv[2], "get_pri2queue", 14))
			qos_get_pri2queue(argc, argv);
		else if (!strncmp(argv[2], "set_weight", 9))
			qos_set_weight(argc, argv);
		else if (!strncmp(argv[2], "get_weight", 9))
			qos_get_weight(argc, argv);
		else
			usage(argv[0]);
	} else if (!strncmp(argv[1], "test_mode", 9)) {
		if (argc < 4)
			usage(argv[0]);
		else
			phy_test_mode(argc, argv);
	} else if (!strncmp(argv[1], "add", 4)) {
		if (argc < 5)
			usage(argv[0]);
		else
			table_add(argc, argv);
	} else if (!strncmp(argv[1], "del", 4)) {
		if (argc < 5)
			usage(argv[0]);
		else
			table_del(argc, argv);
	} else if (!strncmp(argv[1], "port_trunk", 11)) {
		if (argc < 3)
			usage(argv[0]);
		else
			set_port_trunk(argc, argv);
	}else
		usage(argv[0]);

	switch_fini();
	return 0;
}

