******************How to use this driver***************************

1) Execute the Makefile on the console, T_Queue_driver.ko (kernel object module is created)
2) Install the driver with the insmod command. Eg : insmod T_Queue_driver.ko
3) Two device files /dev/Tqueue1 and /dev/Tqueue2 will be created, use these device files to access the device.
4) If you are unable to access the device files change permissions using chmod
5) In some cases the T_Queue_driver.ko is not portable across platforms, so use the Makefile to re-build the module again.
