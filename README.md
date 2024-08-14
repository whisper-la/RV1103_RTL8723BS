# RTL8723BS 蓝牙WIFI


- [RTL8723BS 蓝牙WIFI](#rtl8723bs-蓝牙wifi)
	- [一、开发环境配置](#一开发环境配置)
		- [1.1开发环境](#11开发环境)
		- [1.2 安装依赖环境](#12-安装依赖环境)
		- [1.3 获取SDK](#13-获取sdk)
		- [1.4 编译镜像](#14-编译镜像)
	- [二、固件和驱动文件添加](#二固件和驱动文件添加)
		- [2.1 添加固件](#21-添加固件)
		- [2.2 添加驱动程序](#22-添加驱动程序)
		- [2.3 rkwifibt文件夹加入编译](#23-rkwifibt文件夹加入编译)
	- [三、WIFI配置及使用](#三wifi配置及使用)
		- [3.1 修改内核](#31-修改内核)
			- [3.1.1 配置cfg80211](#311-配置cfg80211)
			- [3.1.2 rfkill](#312-rfkill)
			- [3.1.3 驱动支持](#313-驱动支持)
		- [3.2 buildroot软件包](#32-buildroot软件包)
			- [3.2.1 wpa\_supplicant](#321-wpa_supplicant)
			- [3.2.2 wireless tools](#322-wireless-tools)
			- [3.2.3 iw](#323-iw)
		- [3.3 配置dts](#33-配置dts)
			- [3.3.1 添加节点](#331-添加节点)
			- [3.3.2 追加节点属性](#332-追加节点属性)
			- [3.3.3 sdmmc0检测引脚拉低](#333-sdmmc0检测引脚拉低)
		- [3.4 修改build.sh脚本](#34-修改buildsh脚本)
		- [3.5 WIFI支持](#35-wifi支持)
		- [3.6 使用方法](#36-使用方法)
			- [3.6.1 测试](#361-测试)
			- [3.6.2 修改网络](#362-修改网络)
	- [四、蓝牙的配置及使用](#四蓝牙的配置及使用)
		- [4.1 修改内核](#41-修改内核)
			- [4.1.1 蓝牙子系统](#411-蓝牙子系统)
			- [4.1.2 用户层驱动支持](#412-用户层驱动支持)
			- [4.1.3 HID驱动](#413-hid驱动)
		- [4.2 buildroot软件包](#42-buildroot软件包)
			- [4.2.1 Target options](#421-target-options)
			- [4.2.2 设置外部工具链](#422-设置外部工具链)
			- [4.2.3 bluez](#423-bluez)
			- [4.2.4 dbus](#424-dbus)
			- [4.2.5 expat](#425-expat)
			- [4.2.6 添加bluez依赖，防止报错](#426-添加bluez依赖防止报错)
		- [4.3 使用方法](#43-使用方法)

## 一、官方开发环境配置
下面是官方的开发环境SDK编译方法，如果使用本仓库即不必执行1.3，SDK已经下载
### 1.1开发环境

>- SDK version：V1.3或V1.4    
>- SDK仓库地址：https://gitee.com/LuckfoxTECH/luckfox-pico.git
>- RTL8723：一款WIFI和蓝牙集成的芯片

### 1.2 安装依赖环境

```shell
sudo apt update

sudo apt-get install -y git ssh make gcc gcc-multilib g++-multilib module-assistant expect g++ gawk texinfo libssl-dev bison flex fakeroot cmake unzip gperf autoconf device-tree-compiler libncurses5-dev pkg-config bc python-is-python3 passwd openssl openssh-server openssh-client vim file cpio rsync

```

### 1.3 获取SDK

```shell
git clone https://gitee.com/LuckfoxTECH/luckfox-pico.git
```

### 1.4 编译镜像

```shell
luckfox@luckfox:~$ ./build.sh lunch
You're building on Linux
  Lunch menu...pick the Luckfox Pico hardware version:
  选择 Luckfox Pico 硬件版本:
                [0] RV1103_Luckfox_Pico
                [1] RV1103_Luckfox_Pico_Mini_A
                [2] RV1103_Luckfox_Pico_Mini_B
                [3] RV1103_Luckfox_Pico_Plus
                [4] RV1106_Luckfox_Pico_Pro_Max
                [5] RV1106_Luckfox_Pico_Ultra
                [6] RV1106_Luckfox_Pico_Ultra_W
                [7] custom
Which would you like? [0~7][default:0]: 3
  Lunch menu...pick the boot medium:
  选择启动媒介:
                [0] SD_CARD
                [1] SPI_NAND
Which would you like? [0~1][default:0]: 1
  Lunch menu...pick the system version:
  选择系统版本:
                [0] Buildroot(Support Rockchip official features)
Which would you like? [0~1][default:0]: 0
[build.sh:info] Lunching for Default BoardConfig_IPC/BoardConfig-SPI_NAND-Buildroot-RV1103_Luckfox_Pico_Plus-IPC.mk boards...
[build.sh:info] Running build_select_board succeeded.

luckfox@luckfox:~$  ./build.sh
```



## 二、WIFI使用

- 编译

```shell
./build.sh
```

- 启动WIFI
  
```shell
cd /lib/firmware/rtlwifi/

chmod 777 install_wifi.sh

./install_wifi.sh


```

- 如果需要下次开机WIFI自动启动，但是可能会拖慢开机速度

```shell
echo "./lib/firmware/rtlwifi/install_wifi.sh" >> /etc/init.d/rcS

```

- WIFI支持同一网络下SSH登录

- 修改网络

如果需要修改网络，可以直接将<SDK目录>/external/lib/firmware/rtlwifi/install_wifi.sh脚本中的SSID和PSK分别修改为网络的名称和密码。

## 三、蓝牙使用


- 编译
  
```shell
./build.sh
```

- 启动蓝牙并打开广播模式
  
```shell
cd /lib/firmware/rtlbt/

chmod 777 install_bluetooth.sh

./install_bluetooth.sh
```

- 利用手机可以搜索到当前正在广播的设备
