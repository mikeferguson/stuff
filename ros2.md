# Some ROS2 Notes

## Documentation

 * [Overview](https://index.ros.org/doc/ros2/)
 * [Design Documents](http://design.ros2.org)
 * [Roadmap](https://github.com/ros2/ros2/wiki/Roadmap)
 * [Parameters](http://design.ros2.org/articles/ros_parameters.html)
 * [Logging](https://index.ros.org/doc/ros2/Concepts/Logging/)
 * [Launch](http://design.ros2.org/articles/roslaunch.html)
 * [rclpy](http://docs.ros2.org/latest/api/rclpy/index.html)

## Cheat Sheet

 * ros2 run \<pkg\> \<node\>
 * ros2 node list
 * ros2 topic list -t
 * ros2 param list
 * colcon build --cmake-clean-cache

## Enhancements/Changes from ROS1

 * No ROS master.
   * ROS_DOMAIN_ID environment variable
   * Initial annoucement when node starts up
   * Periodic announcement after that
   * Nodes announce when they go offline
 * _catkin_ has been replaced by _ament_.
   * package.xml remains. Anything containing package.xml is an ament package
     regardless of the underlying build system.
   * New _ament_cmake_ build_type introduced. Can still build pure _cmake_ packages.
   * _ament_package_ - python package contains utilities for using ament
 * Locating/searching packages is provided by [colcon](http://design.ros2.org/articles/build_tool.html)
 * Internal APIs are C.
   * _rwm_ - ROS->DDS
   * _rcl_ - ROS->ClientLibrary. _rclcpp_, _rclpy_ are built on top of _rcl_.
     This provide core pub/sub/service - but not an execution model (threading
     is implemented natively in the client library language).
 * Messages support default values (a third element in the IDL)
 * No parameter server - all parameters are node-local
   * This also means there is no tilde for local parameters - [all parameters are local](https://answers.ros.org/question/317929/retrieving-parameters-from-nodes-rclpy/).

## Tutorials

 * [Hands On (roscon 2018)](https://vimeo.com/292693129). [GitHub](https://github.com/Karsten1987/confbot_robot)
 * [ROS2 Launch (roscon 2018)](https://vimeo.com/292699162)

## Tips and Tricks

 * Connections will only be made when QoS is compatible [QoS Tutorial](https://index.ros.org/doc/ros2/Tutorials/Quality-of-Service/)
 * [Tuning FastRTPS (roscon 2017)](https://roscon.ros.org/2017/presentations/ROSCon%202017%20ROS2%20Fine%20Tuning.pdf)
 * [Apex.AI Performance Testing](https://gitlab.com/ApexAI/performance_test/)
 * [You should always declare parameters, but here's a workaround](https://answers.ros.org/question/325960/do-i-always-have-to-declare-parameters/)
 * Uninstalling python3-colcon-output will cause all build/test output to go to screen

## Examples

 * [rclpy parameters example](https://answers.ros.org/question/348149/confision-about-ros2-rclpy-parameter/)
 * [porting a simple python node](https://github.com/mikeferguson/etherbotix_python/commit/3e7693584959f085393b2d0994ef76cffafcc916)

## Productivity

 * [Live Sync Daemon](https://blog.roverrobotics.com/ros-2-dev-tip-sync-your-code-with-lsyncd/)

## Other notes

 * Missing this:
   ```
   <export>
    <build_type>ament_cmake</build_type>
   </export>
   ```
   results in this:
   ```
   CMake Error at /usr/share/cmake-3.16/Modules/FindPackageHandleStandardArgs.cmake:146 (message):
     Could NOT find FastRTPS (missing: FastRTPS_INCLUDE_DIR FastRTPS_LIBRARIES)

   ```
  * Mixing CPP/Python has issues [answers.ros.org](https://discourse.ros.org/t/mixed-python-cpp-ament-package/1452/9)
