PKG_TARGET += build-driver
PKG_CLEAN_TARGET += build-driver-clean

ifeq ($(M_OUT_DIR),)
# TODO
$(error Please build on upper directory)
endif

all: $(PKG_TARGET)
	@echo "build bt driver done"
	@cp ./rtk_hciattach/rtk_hciattach $(M_OUT_DIR);
	@cp ./bluetooth_uart_driver/hci_uart.ko $(M_OUT_DIR);
	@cp start_bt.sh $(M_OUT_DIR);

build-driver:
	@make -C bluetooth_uart_driver
	#@make -C bluetooth_usb_driver
	@make -C rtk_hciattach

build-driver-clean:
	@make -C bluetooth_uart_driver clean
	#@make -C bluetooth_usb_driver clean
	@make -C rtk_hciattach clean

clean: $(PKG_CLEAN_TARGET)
