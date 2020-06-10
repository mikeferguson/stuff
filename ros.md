# Common Commands

rosdep install --from-paths src --ignore-src --rosdistro=melodic -y

rosdep install --from-paths src --ignore-src --rosdistro=noetic -y

catkin_make_isolated -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS_RELEASE=-O3

[Setting bloom/git to always use ssh](https://answers.ros.org/question/234494/diagnosing-issues-with-bloom-github-two-factor-authentication/)
