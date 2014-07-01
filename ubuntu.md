== Trusty ==

== Precise ==
Installation notes:

 * Install server edition
 * To get gnome:
 
   sudo apt-get install lightdm gnome-session-panel gnome-core
 
 * To get the appearance:

   sudo apt-get install light-themes
   Settings -> Appearance -> Theme -- switch to "ambiance"

 * Other packages:

   sudo apt-get install nvidia-current gedit update-manager
   sudo apt-get install gstreamer0.10-plugins-ugly gstreamer0.10-ffmpeg
   sudo apt-get install openssh-server

 * On desktops, might be useful to:
 
   sudo apt-get purge network-manager

== SSH Settings ==
 * Update /etc/ssh/sshd_config (the "banner" section)
