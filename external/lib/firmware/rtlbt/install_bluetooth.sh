# 加载驱动
insmod /oem/usr/ko/hci_uart.ko

# 启动dbus进程
killall dbus-daemon
rm /run/messagebus.pid
dbus-daemon --system --print-pid --print-address

# 初始化绑定ttyS4 波特率115200 h5协议
/oem/usr/ko/rtk_hciattach -n -s 115200 /dev/ttyS4 rtk_h5 &
sleep 5
echo -e "\e[32m hci0 bond the ttyS4 !!! \e[0m"

hciconfig -a

# 开启射频开关
if cat /sys/class/rfkill/rfkill1/uevent |grep hci0 >/dev/null 
then
        echo 1 > /sys/class/rfkill/rfkill1/state
else
	echo 1 > /sys/class/rfkill/rfkill2/state
fi

echo -e "\e[32m hci0 rfkill has stated !!!\e[0m"

# 启动蓝牙
output3=`hciconfig hci0 up` 
hciconfig hci0 name "RFIDtest"
echo -e "\e[32m hci0 started !!!\e[0m"
hciconfig hci0 piscan

/usr/libexec/bluetooth/bluetoothd -n -d &
sleep 5

hcitool dev
# 开启广播
hciconfig hci0 leadv
echo -e "\e[32m bluetooth is advertising !!!\e[0m"
