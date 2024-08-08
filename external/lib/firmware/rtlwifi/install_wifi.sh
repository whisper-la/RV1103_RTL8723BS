#!/bin/bash
echo "initing wifi ......"

WIFI="wlan0"
ACC_SETTING="/etc/wpa_supplicant.conf"
SOCKET="/var/run/wpa_supplicant"
SSID="wpdata_sec"
PSK="31415926"


# 进入板子的根文件系统
# 安装驱动模块
cd /oem/usr/ko
insmod libarc4.ko
insmod cfg80211.ko
insmod mac80211.ko
insmod r8723bs.ko

if ifconfig -a |grep wlan0 >/dev/null 
then
        ifconfig wlan0 up
fi

cat << EOF >${ACC_SETTING}
ctrl_interface=${SOCKET}
ctrl_interface_group=0
ap_scan=1
network={
    ssid="${SSID}"
    scan_ssid=1
    key_mgmt=WPA-PSK
    pairwise=CCMP
    group=CCMP
    psk="${PSK}"
    priority=3
}
EOF

mkdir -p $SOCKET

output1=`wpa_supplicant -B -c ${ACC_SETTING} -i $WIFI`

output2=`udhcpc -i $WIFI &`

echo "wifi connect success !!!"


