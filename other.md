### Ubuntu 12.04 install (no unity)

        install server version (no gui)
        sudo apt-get install lightdm gnome-session gnome-panel gnome-core light-themes

### projector notes

 * Need to install new versions of ffmpeg (http://ppa.launchpad.net/jon-severinsson/ffmpeg/ubuntu)
 * git-buildpackage -us -uc --git-ignore-branch from https://github.com/mikeferguson/libam7xxx branch debian-ubuntu

 * original source: http://git.ao2.it/libam7xxx.git/
 * http://ao2.it/en/news/2013/07/28/libam7xxx-014-released

### wireshark

        udp.dstport == 1000 && frame.time_relative < 1.0

### Openni2

Setting to verbose logging: edit /etc/openni2/Openni.ini to set verbosity to 0, be sure to set LogToConsole or LogToFile to 1 to get outputs.

### SSD Life Info

    sudo apt-get install smartmontools
    sudo smartctl -a /dev/sda | grep Media_Wearout_Indicator
