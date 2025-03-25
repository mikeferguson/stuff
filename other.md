## Fixing STM Cube output

        dos2unix $(find . -type f)

## Debugging Wireless

Graphical tool to view connection:

        sudo apt-get install wavemon
        wavemon

Setting network from command line

        nmcli con up id SSID iface wlan0

## wireshark

        udp.dstport == 1000 && frame.time_relative < 1.0

## Debugging libusb

Environment variable LIBUSB_DEBUG can be set to levels 0 (none) to 4 (DEBUG). Default is 0,
debug will probably destroy your computer.

## Openni2

Setting to verbose logging: edit /etc/openni2/Openni.ini to set verbosity to 0, be sure to set LogToConsole or LogToFile to 1 to get outputs.

## SSD Life Info

    sudo apt-get install smartmontools
    sudo smartctl -a /dev/sda | grep Media_Wearout_Indicator
