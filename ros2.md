# Some ROS2 Notes

## Documentation

 * [Overview](https://index.ros.org/doc/ros2/)
 * [Design Documents](http://design.ros2.org)
   * [Parameters](http://design.ros2.org/articles/ros_parameters.html)
   * [Logging](https://index.ros.org/doc/ros2/Concepts/Logging/)
   * [Launch](http://design.ros2.org/articles/roslaunch.html)
 * API
   * [rclcpp](http://docs.ros2.org/foxy/api/rclcpp/index.html)
   * [rclpy](http://docs.ros2.org/latest/api/rclpy/index.html)
 * [About QoS](https://index.ros.org/doc/ros2/Concepts/About-Quality-of-Service-Settings/)

## Cheat Sheet

 * ros2 run \<pkg\> \<node\>
 * ros2 node list
 * ros2 topic list -t
 * ros2 topic info \<topic\> --verbose (to get QoS)
 * ros2 param list
 * colcon build --cmake-clean-cache
 * colcon <verb> --packages-select pkg_name (verbs include build/test)
 * colcon <verb> --event-handlers console_direct+ (to get everything to screen)

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
 * [Porting asyncspinners](https://github.com/ros2/rclcpp/issues/335)
 * [Building with debug symbols in colcon](https://answers.ros.org/question/320252/ros2-colcon-debug-symbols-for-use-with-ddd/)
 * [ROS2 launch prefix](https://answers.ros.org/question/343326/ros2-prefix-in-launch-file/)
 * [Discussion on rclcpp::Time vs std::chrono](https://discourse.ros.org/t/ros-2-time-vs-std-chrono/6293/8)

## Examples

 * [rclpy parameters example](https://answers.ros.org/question/348149/confision-about-ros2-rclpy-parameter/)
 * [porting a simple python node](https://github.com/mikeferguson/etherbotix_python/commit/3e7693584959f085393b2d0994ef76cffafcc916)
 * [ZED driver](https://www.stereolabs.com/docs/ros2/lifecycle/)
   * has a nice launch.py example, including setting the log format.
 * [ROS2 Demos Repo](https://github.com/ros2/demos)

## rclcpp

 * [components](https://index.ros.org/doc/ros2/Tutorials/Composition/)

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
  * [Migration Question](https://answers.ros.org/question/354216/switching-to-ros2-foxy-or-staying-on-ros1-hurdles-caveats/)

## Bugs / Deficiencies

 * [No subscriber connect/disconnect callbacks](https://answers.ros.org/question/286069/implementing-subscriberstatuscallback-within-ros2/)
 * [TF delays in RVIZ](https://github.com/ros2/rviz/issues/359)
 * Loading meshes in RVIZ takes way longer than in ROS1 (no issue yet)
 * [RVIZ lacks DepthCloudDisplay](https://github.com/ros2/rviz/issues/92)
 * RVIZ should automagically determine QoS of incoming topic. References:
   * [Adaptive QoS for rosbag2](https://github.com/ros2/rosbag2/pull/343/files)
  
## Future Features
 * [Add color selection to costmap view](https://github.com/ros2/rviz/issues/460)
