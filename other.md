### projector notes

 * Need to install new versions of ffmpeg (http://ppa.launchpad.net/jon-severinsson/ffmpeg/ubuntu)
 * git-buildpackage -us -uc --git-ignore-branch from https://github.com/mikeferguson/libam7xxx branch debian-ubuntu

 * original source: http://git.ao2.it/libam7xxx.git/
 * http://ao2.it/en/news/2013/07/28/libam7xxx-014-released

### wireshark

        udp.dstport == 1000 && frame.time_relative < 1.0

### Debugging libusb

Environment variable LIBUSB_DEBUG can be set to levels 0 (none) to 4 (DEBUG). Default is 0,
debug will probably destroy your computer.

### Openni2

Setting to verbose logging: edit /etc/openni2/Openni.ini to set verbosity to 0, be sure to set LogToConsole or LogToFile to 1 to get outputs.

### SSD Life Info

    sudo apt-get install smartmontools
    sudo smartctl -a /dev/sda | grep Media_Wearout_Indicator

### Displaylink output
```
[ 1550.270941] usb 2-2: new high-speed USB device number 9 using xhci_hcd
[ 1550.287214] usb 2-2: New USB device found, idVendor=058f, idProduct=6254
[ 1550.287224] usb 2-2: New USB device strings: Mfr=0, Product=0, SerialNumber=0
[ 1550.287790] hub 2-2:1.0: USB hub found
[ 1550.287847] hub 2-2:1.0: 4 ports detected
[ 1550.575077] usb 2-2.2: new high-speed USB device number 10 using xhci_hcd
[ 1550.595826] usb 2-2.2: New USB device found, idVendor=17e9, idProduct=401a
[ 1550.595836] usb 2-2.2: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[ 1550.595842] usb 2-2.2: Product: mimo inc
[ 1550.595846] usb 2-2.2: Manufacturer: DisplayLink
[ 1550.595850] usb 2-2.2: SerialNumber: 7041923
[ 1550.597735] [drm] vendor descriptor length:23 data:23 5f 01 00 21 00 04 04 07 00 01
[ 1550.684363] udl 2-2.2:1.0: fb1: udldrmfb frame buffer device
[ 1550.684376] [drm] Initialized udl 0.0.1 20120220 on minor 1
[ 1550.685590] hid-generic 0003:17E9:401A.0004: hiddev0,hidraw2: USB HID v1.10 Device [DisplayLink mimo inc] on usb-0000:00:14.0-2.2/input1
[ 1550.771055] usb 2-2.3: new full-speed USB device number 11 using xhci_hcd
[ 1550.792240] usb 2-2.3: New USB device found, idVendor=1ac7, idProduct=0001
[ 1550.792249] usb 2-2.3: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[ 1550.792254] usb 2-2.3: Product: USB Touchpanel
[ 1550.792258] usb 2-2.3: Manufacturer: e2i Technology, Inc.
[ 1550.792262] usb 2-2.3: SerialNumber: L000000000
[ 1550.795334] input: e2i Technology, Inc. USB Touchpanel as /devices/pci0000:00/0000:00:14.0/usb2/2-2/2-2.3/2-2.3:1.0/input/input13
```
