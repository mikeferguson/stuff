### projector notes

 * Need to install new versions of ffmpeg (http://ppa.launchpad.net/jon-severinsson/ffmpeg/ubuntu)
 * git-buildpackage -us -uc --git-ignore-branch from https://github.com/mikeferguson/libam7xxx branch debian-ubuntu

 * original source: http://git.ao2.it/libam7xxx.git/
 * http://ao2.it/en/news/2013/07/28/libam7xxx-014-released

### wireshark

        udp.dstport == 1000 && frame.time_relative < 1.0
