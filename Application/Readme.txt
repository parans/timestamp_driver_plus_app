************Description of the application************************************************

These source files consist of the application logic for testing Token_Queue_driver

The Sender sends data to the device file (/dev/Tqueue1) to be time stamped.
The Router reads data from the device file (/dev/Tqueue1) and prints it on screen and writes it to another device file(/dev/Tqueue2).
The receiver reads data from the device file (/dev/Tqueue2) and prints it on the console.

2 threads each of sender, router, receiver is created and these threads perform their respective actions for n times.

************How to run the application**********************

Execute Makefile on the console. (Eg : $make)
An executable will be created called "Token_SRR".
Executable does not take any argument, it always generates 100 tokens.
Ex: $./Token_SRR
