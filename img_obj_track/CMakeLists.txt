cmake_minimum_required(VERSION 2.8.3)
project(img_obj_track)

find_package(catkin REQUIRED COMPONENTS
  cv_bridge
  roscpp
  std_msgs
  rsu_msgs
)


catkin_package(

)


include_directories(
# include
  ${catkin_INCLUDE_DIRS}
)


add_executable(${PROJECT_NAME}_node src/img_obj_track.cpp)

target_link_libraries(${PROJECT_NAME}_node
   ${catkin_LIBRARIES}
)


