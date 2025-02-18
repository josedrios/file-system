make all
sudo insmod driver.ko
sudo mknod /dev/driver c 415 0
sudo chmod 666 /dev/driver