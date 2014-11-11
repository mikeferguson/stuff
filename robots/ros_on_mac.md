## Indigo on 10.9

 * Installed October of 2014.
 * Had to clean up custom install of sip (remove /usr/include/python2.7/sip.h header)
 * Manually removed:
   * yaml-cpp-0.3
   * ogre
 * Manually installed:
   * yaml-cpp
   * ogre-1.9 (https://raw.githubusercontent.com/hgaiser/homebrew-simulation/master/ogre1.9.rb)
 * rosinstall generator for desktop
   * switched to osx_ogre_1_9 branch of rviz

References:
 * [Ticket discussing ogre 1.7/8/9 issues and mesh loading among others](https://github.com/ros-visualization/rviz/issues/782)
 * [Main ticket tracking indigo on OSX](https://github.com/ros/rosdistro/issues/4597)

Installing Moveit (in progress):
 * source ~/ros/indigo_ws/install_isolated/setup.bash
 * cd ~/ros/indigo_moveit/
 * rosinstall_generator moveit_ros --deps --exclude RPP > moveit.rosinstall
 * wstool init -j8 src moveit.rosinstall
 * rosdep install --from-paths src --ignore-src --rosdistro indigo
 * compiled, not yet entirely working

## Hydro on 10.9

Installed in December of 2013

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
