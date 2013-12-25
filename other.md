### Ubuntu 12.04 install (no unity)

        install server version (no gui)
        sudo apt-get install lightdm gnome-session gnome-session gnome-panel gnome-core light-themes

### projector notes

 * Need to install new versions of ffmpeg (http://ppa.launchpad.net/jon-severinsson/ffmpeg/ubuntu)
 * git-buildpackage -us -uc --git-ignore-branch from https://github.com/mikeferguson/libam7xxx branch debian-ubuntu

 * original source: http://git.ao2.it/libam7xxx.git/
 * http://ao2.it/en/news/2013/07/28/libam7xxx-014-released

### wireshark

        udp.dstport == 1000 && frame.time_relative < 1.0
        
### ROS on Mac OS X 10.9

References:
 * http://wiki.ros.org/hydro/Installation/OSX/Homebrew/Source
 * http://answers.ros.org/question/94771/building-ros-on-osx-109-solution/

        had to link freetype2 includes to /usr/local/include/freetype
        had to install sphinx via pip (for pcl)
        had to install XQuartz manually via installer
        manually installed SIP from http://www.riverbankcomputing.co.uk/software/sip/download
        
Notes:
 * needed to manually install SIP since we aren't using python from homebrew and pip/easy_install both seem to fail.

Patches applied:
 * http://answers.ros.org/question/112856/installing-pyqwt-on-mavericks/
 * http://answers.ros.org/question/112866/error-compiling-eigen_msg-on-osx-109/
 * https://github.com/kyonifer/ros-macports/commit/96617b4d7292558bdbe646953302ba6263d849c4 (added the #include <ios>)
 * https://github.com/trainman419/ros_comm/commit/87a79b7da444833c8557c28b1163529c08943e6c
