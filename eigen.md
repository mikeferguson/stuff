# Eigen Notes

 * Eigen::Vector3d, Affine3d are used for point, pose.
 * To get <roll, pitch, yaw> from Quaterniond:
 
       Eigen::Vector3d angles = q.matrix().eulerAngles(0,1,2);

 * http://eigen.tuxfamily.org/dox/group__TutorialGeometry.html
 * Insides of moveit_core::RobotState are a good tutorial.
