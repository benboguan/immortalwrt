#
# Copyright (C) 2016 MediaTek
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/version.mk

PKG_NAME:=wifi-profile
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0

PKG_KCONFIG:= \
	first_card \
	first_card_name \
	second_card \
	second_card_name \
	third_card \
	third_card_name \
	WIFI_NORMAL_SETTING \
	WIFI_QUICK_SETTING \
	MTK_WIFI_SKU_TYPE

PKG_CONFIG_DEPENDS:=$(foreach c, $(PKG_KCONFIG),$(if $(CONFIG_$c),CONFIG_$(c)))

include $(INCLUDE_DIR)/package.mk

define Package/wifi-profile
  SECTION:=MTK Properties
  CATEGORY:=MTK Properties
  SUBMENU:=Drivers
  DEPENDS:=
  TITLE:=Build WiFi l1profile.data on demand & install wifi l2 profile.
  VERSION:=$(PKG_RELEASE)-$(REVISION)
  MENU:=1
endef

define Package/wifi-profile/description
  This package helps to build l1profile on demand & install wifi l2 profile.
endef

define Package/wifi-profile/config
endef

define Build/Prepare
endef

define Build/Compile
endef

define Package/wifi-profile/install
	$(INSTALL_DIR) $(1)/etc/init.d/
	$(INSTALL_DIR) $(1)/lib/wifi/
	$(INSTALL_DIR) $(1)/etc/wireless/mediatek/
	echo $(PKG_VERSION) > $(1)/etc/wireless/mediatek/version
	$(INSTALL_BIN) ./files/firmware.sh $(1)/etc/init.d/

	# Example combination mt7629.
	if [ "$$(CONFIG_first_card_name)" = "MT7629" ] ; then \
		$(INSTALL_BIN) ./files/mt7629/*7629*.* $(1)/etc/wireless/mediatek/; \
		if [ "$$(__CONFIG_MTK_MAP_SUPPORT)" = "y" ] ; then \
			$(INSTALL_BIN) ./files/mt7629/map.1.b0.2g.dat $(1)/etc/wireless/mediatek/mt7629.1.b0.2g.dat; \
			$(INSTALL_BIN) ./files/mt7629/map.1.b1.5g.dat $(1)/etc/wireless/mediatek/mt7629.1.b1.5g.dat; \
		fi; \
	fi

	if [ "$$(CONFIG_first_card_name)" = "MT7626" ] ; then \
		$(INSTALL_BIN) ./files/mt7629/*7626*.* $(1)/etc/wireless/mediatek/; \
	fi

	# Example combination mt7622 + mt7615e/a + mt7615e/a.
	if [ "$$(CONFIG_first_card_name)" = "MT7622" ] ; then \
		$(INSTALL_BIN) ./files/mt7622.1.dat $(1)/etc/wireless/mediatek/; \
		if [ "$$(CONFIG_MTK_MAP_SUPPORT)" = "y"  ] ; then \
			$(INSTALL_BIN) ./files/mt7622_map.1.dat $(1)/etc/wireless/mediatek/mt7622.1.dat; \
		fi; \
		if [ "$$(CONFIG_second_card_name)" = "MT7615" ] ; then \
			$(INSTALL_BIN) ./files/mt7615e.1.dat $(1)/etc/wireless/mediatek/; \
			if [ "$$(CONFIG_MTK_MAP_SUPPORT)" = "y"  ] ; then \
                        	$(INSTALL_BIN) ./files/mt7615e_map.1.dat $(1)/etc/wireless/mediatek/mt7615e.1.dat; \
                	fi; \
			$(INSTALL_BIN) ./files/mt7615a.b0.5g.dat $(1)/etc/wireless/mediatek/mt7615a.1.b0.5g.dat; \
			$(INSTALL_BIN) ./files/mt7615a.b1.5g.dat $(1)/etc/wireless/mediatek/mt7615a.1.b1.5g.dat; \
		fi; \
		if [ "$$(CONFIG_third_card_name)" = "MT7615" ] ; then \
			$(INSTALL_BIN) ./files/mt7615e.2.dat $(1)/etc/wireless/mediatek/; \
			if [ "$$(CONFIG_MTK_MAP_SUPPORT)" = "y"  ] ; then \
                        	$(INSTALL_BIN) ./files/mt7615e_map.2.dat $(1)/etc/wireless/mediatek/mt7615e.2.dat; \
                	fi; \
			$(INSTALL_BIN) ./files/mt7615a.b0.5g.dat $(1)/etc/wireless/mediatek/mt7615a.2.b0.5g.dat; \
			$(INSTALL_BIN) ./files/mt7615a.b1.5g.dat $(1)/etc/wireless/mediatek/mt7615a.2.b1.5g.dat; \
		fi; \
		$(INSTALL_BIN) ./files/mt7615e*.bin $(1)/etc/wireless/mediatek/; \
	fi

	# Example combination mt7623/mt7621 + mt7615e/a + mt7615e/a.
	if [ "$$(CONFIG_first_card_name)" = "MT7615" ] ; then \
		$(INSTALL_DATA) ./files/7615.l1profile.dat $(1)/etc/wireless/l1profile.dat ; \
		if [ "$$(CONFIG_PACKAGE_libmapd)" = "y" ]; then \
			$(INSTALL_BIN) ./files/mt7615e_map.1.dat $(1)/etc/wireless/mediatek/mt7615e.1.dat; \
		else \
			$(INSTALL_BIN) ./files/mt7615e.1.dat $(1)/etc/wireless/mediatek/; \
		fi; \
		$(INSTALL_BIN) ./files/mt7615a.b0.5g.dat $(1)/etc/wireless/mediatek/mt7615a.1.b0.5g.dat; \
		$(INSTALL_BIN) ./files/mt7615a.b1.5g.dat $(1)/etc/wireless/mediatek/mt7615a.1.b1.5g.dat; \
		if [ "$$(CONFIG_second_card_name)" = "MT7615" ] ; then \
			if [ "$$(CONFIG_PACKAGE_libmapd)" = "y" ]; then \
				$(INSTALL_BIN) ./files/mt7615e_map.2.dat $(1)/etc/wireless/mediatek/mt7615e.2.dat; \
			else \
				$(INSTALL_BIN) ./files/mt7615e.2.dat $(1)/etc/wireless/mediatek/; \
			fi; \
			$(INSTALL_BIN) ./files/mt7615a.b0.5g.dat $(1)/etc/wireless/mediatek/mt7615a.2.b0.5g.dat; \
			$(INSTALL_BIN) ./files/mt7615a.b1.5g.dat $(1)/etc/wireless/mediatek/mt7615a.2.b1.5g.dat; \
		fi; \
		$(INSTALL_BIN) ./files/mt7615e*.bin $(1)/etc/wireless/mediatek/; \
	fi

	# Example combination mt7622 + axe.
	if [ "$$(CONFIG_first_card_name)" = "AXE" ] ; then \
		$(INSTALL_BIN) ./files/axe.1.dat $(1)/etc/wireless/mediatek/; \
	fi

	# Example combination for MT7915
	if [ "$$(CONFIG_first_card_name)" = "MT7915" -o "$$(CONFIG_second_card_name)" = "MT7915" \
		-o "$$(CONFIG_third_card_name)" = "MT7915" ] ; then \
		$(INSTALL_BIN) ./files/mt7915/* $(1)/etc/wireless/mediatek/; \
	fi
	# Example combination for MT7915D
	if [ "$$(CONFIG_first_card_name)" = "MT7915D" -o "$$(CONFIG_second_card_name)" = "MT7915D" ] ; then \
		$(INSTALL_BIN) ./files/mt7915D/* $(1)/etc/wireless/mediatek/; \
		$(INSTALL_DATA) ./files/7915d.l1profile.dat $(1)/etc/wireless/l1profile.dat ; \
	fi

	# Example mt7615D
	if [ "$$(CONFIG_first_card_name)" = "MT7615D" -o "$$(CONFIG_second_card_name)" = "MT7615D" ] ; then \
		$(INSTALL_BIN) ./files/mt7615D/*.* $(1)/etc/wireless/mediatek/; \
		$(INSTALL_DATA) ./files/7615d.l1profile.dat $(1)/etc/wireless/l1profile.dat ; \
	fi
	# Example mt7613/mt7663
	if [ "$$(CONFIG_first_card_name)" = "MT7663" -o "$$(CONFIG_second_card_name)" = "MT7663" ] ; then \
		$(INSTALL_BIN) ./files/mt7663/*.* $(1)/etc/wireless/mediatek/; \
	fi
endef

$(eval $(call BuildPackage,wifi-profile))
