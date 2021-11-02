cmake_minimum_required(VERSION 2.8.3)
project(motor)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
)

aux_source_directory(./src DIR_SRCS)

catkin_package(
  CATKIN_DEPENDS roscpp rospy std_msgs
)

include_directories(${catkin_INCLUDE_DIRS})

add_executable(motor ${DIR_SRCS})
target_link_libraries(motor ${catkin_LIBRARIES})
