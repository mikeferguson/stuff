Colored Collada Files from STL:

 * Need blender 2.7 (sudo add-apt-repository ppa:irie/blender)
 * make UV (edit mode)
   * under shading/UVs... Unwrap ... Smart UV Project
   * Set Island Margin to space parts out (0.005 works well)
 * paint the UV
 * add a material, add a texture, select the UV, make sure to select "UV Map"
 * under material, set "emission" to 0.15 for best results in RVIZ and Gazebo
 * export Collada (be sure to check export UV, export Materials)
 * In order for gazebo to actually be lit, edit the collada and change ambient to "0.5 0.5 0.5 1"
