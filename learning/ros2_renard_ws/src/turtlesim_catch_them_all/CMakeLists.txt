cmake_minimum_required(VERSION 3.8)
project(turtlesim_catch_them_all)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(turtlesim REQUIRED)
find_package(my_robot_interfaces REQUIRED)
find_package(geometry_msgs REQUIRED)

# Turtle spawner node
add_executable(turtle_spawner_node src/turtle_spawner.cpp)
ament_target_dependencies(turtle_spawner_node
  rclcpp
  turtlesim
  my_robot_interfaces
)

# Turtle controller node
add_executable(turtle_controller_node src/turtle_controller.cpp)
ament_target_dependencies(turtle_controller_node
  rclcpp
  turtlesim
  my_robot_interfaces
  geometry_msgs
)

install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}/
)

install(TARGETS
  turtle_spawner_node
  turtle_controller_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
