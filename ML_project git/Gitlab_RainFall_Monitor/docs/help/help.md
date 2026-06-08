## GENERAL
* OS installation is crucial, as any errors during this stage can have serious consequences.
* The error when building WiringPi library often occurs due to missing packages or files in the Linux system. This may happen during the OS installation or if the file system wasn’t updated properly during a system update.if so,
    * try reinstalling OS
    * if make is present in system (installing make)
        https://www.makeuseof.com/how-to-fix-make-command-not-found-error-ubuntu/
* if there is an error like "error writing to file -write (28:no space left on device)"
    * check how much space is left on your partitions using command
        * df -h
    * check what makes the memory full.
        * It could be a service spamming the log with errors.check this usinng the command
            * sudo du -sh /var/log/*
        * if syslogs is the cause of memory fill.then remove it using the following command
            * sudo truncate -s 0 /var/log/syslog
    * then free up some space by removing unnecessary data or packages       
* Raspberry Pi 4 Model B uses BCM2711 Broadcom chip
    * Processor: Quad-core Cortex-A72 (ARM v8) 64-bit SoC @ 1.5 GHz.

    * Memory: Accesses up to 8GB LPDDR4-2400 SDRAM (depending on model)

    * Caches: 32kB data + 48kB instruction L1 cache per core. 1MB L2 cache.
    * [BCM2711 datasheet](BCM2711-datasheet.pdf)

## PERIPHERALS
* UART in python and c: https://www.electronicwings.com/raspberry-pi/raspberry-pi-uart-communication-using-python-and-c
* Enabling UART for battery monitoring: https://www.electronicwings.com/raspberry-pi/raspberry-pi-uart-communication-using-python-and-c        
* Default state of gpio pins in raspberry pi : https://roboticsbackend.com/raspberry-pi-gpios-default-state/

## NETWORKS
* how to ON/OFF ethernet/wlan: https://stackoverflow.com/questions/23487728/ethernet-disabling-in-raspberry-pi
* issue: IP V4 is not assigning to ethernet(V6 is present): https://www.freshblurbs.com/blog/2022/08/07/fix-eth0-rpi-ubuntu.html
* Add wifi credentials and ethernet details to the existing system
    * sudo vim  /etc/netplan/50-cloud-init.yaml
    * Add the following content to the file (use Vim editor to make sure consistent indentation)
``` bash
network:
    ethernets:
        eth0:
            dhcp4: true
            optional: true
    version: 2
    wifis:
        wlan0:
            dhcp4: true
            optional: true
            access-points:
                "wifi_name":
                        password: "password"

 ``` 	

    * Then run ,to know if it is working
	    * sudo netplan generate
        * sudo netplan apply	

## INSTALLATION
### 1. Install the WiringPi library 

The [WiringPi](https://github.com/WiringPi/WiringPi) library provides the Raspberry Pi GPIO interface. Follow the instructions in that repository or do the following.

```bash
# Clone the repository 
$ git clone https://github.com/WiringPi/WiringPi.git 

# Access the wiringPi folder 
$ cd WiringPi 

# Build the library
$ ./build 
```
### 2. Compile [LoraWANPi](https://github.com/lucasmaziero/lmic-rpi-fox.git) 

```bash
# Access the lmic_rpi folder 
$ cd /home/pi/raingauge/src/lmic_rpi/examples/ttn-abp-send 

# Make the project 
# This will generate the executable for LoraWAN communication
$ make 

# Add the executable to system path
$ nano ~/.bashrc

# Appened the following line to the end of .bashrc file 
$ export PATH="$PATH:/home/pi/raingauge/src/lmic_rpi/examples/ttn-abp-send"

# to avoid rebooting after change
$ source ~/.bashrc

# Usage
# LED flag (0/1) can be used as an indication for data sending
$ ttn-abp-send <DevAddr> <Nwkskey> <Appskey> <Rain_mm> <solar_V> <battery_V> <solar_I> <battery_I> <LED_FLAG>
```

### 3. Add device to zerotier
Follow the instructions on [Zerotier for Raspberry Pi Tutorial](https://pimylifeup.com/raspberry-pi-zerotier/). Go to  [Zerotier](https://my.zerotier.com/) platform and login with the credentials shared via email/open project to monitor/connect to device IPs. 

## RUN A SCRIPT ON STARTUP IN LINUX

### Run the script as service
* how to run a script on startup in Linux : https://www.tutorialspoint.com/run-a-script-on-startup-in-linux
* [another link](service_in_linux.md)