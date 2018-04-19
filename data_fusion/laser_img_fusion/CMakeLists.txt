cmake_minimum_required(VERSION 2.8.3)
project(laser_img_fusion)


find_package(catkin REQUIRED COMPONENTS
  cv_bridge
	rsu_msgs
  roscpp
  std_msgs
  tf
)
find_package(OpenCV REQUIRED)


catkin_package(

)

include_directories(
# include
  ${catkin_INCLUDE_DIRS}
)


add_executable(${PROJECT_NAME}_node src/laser_img_fusion_node.cpp)


target_link_libraries(${PROJECT_NAME}_node
   ${catkin_LIBRARIES}
   ${OpenCV_LIBRARIES}
 )

