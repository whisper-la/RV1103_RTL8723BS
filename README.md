# RTL8723BS 蓝牙WIFI
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
