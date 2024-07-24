cd /oem/usr/ko

sleep 2

insmod hci_uart.ko

sleep 2

./rtk_hciattach -n -s 115200 /dev/ttyS4 rtk_h5 &

sleep 3

echo 1 > /sys/class/rfkill/rfkill0/state

hciconfig hci0 up

sleep 1
