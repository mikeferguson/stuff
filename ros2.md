# Some ROS2 Notes

## Documentation

 * [Overview](https://index.ros.org/doc/ros2/)
 * [Design Documents](http://design.ros2.org)
 * [Roadmap](https://github.com/ros2/ros2/wiki/Roadmap)
 * [Parameters](http://design.ros2.org/articles/ros_parameters.html)
 * [Logging](https://index.ros.org/doc/ros2/Concepts/Logging/)
 * [Launch](http://design.ros2.org/articles/roslaunch.html)

## Cheat Sheet

 * ros2 run <pkg> <node>
 * ros2 node list
 * ros2 topic list -t
 * ros2 param list
 * colcon bbuild --cmake-clean-cache

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

## Tutorials

 * [Hands On (roscon 2018)](https://vimeo.com/292693129). [GitHub](https://github.com/Karsten1987/confbot_robot)
 * [ROS2 Launch (roscon 2018)](https://vimeo.com/292699162)

## Tips and Tricks

 * Connections will only be made when QoS is compatible [QoS Tutorial](https://index.ros.org/doc/ros2/Tutorials/Quality-of-Service/)
 * [Tuning FastRTPS (roscon 2017)](https://roscon.ros.org/2017/presentations/ROSCon%202017%20ROS2%20Fine%20Tuning.pdf)
 * [Apex.AI Performance Testing](https://gitlab.com/ApexAI/performance_test/)

## Productivity

 * [Live Sync Daemon](https://blog.roverrobotics.com/ros-2-dev-tip-sync-your-code-with-lsyncd/)
