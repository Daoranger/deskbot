cmake_minimum_required(VERSION 3.8)
project(my_activity_four_pkg)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(example_interfaces REQUIRED)
find_package(my_robot_interfaces REQUIRED)

add_executable(battery src/battery.cpp)
ament_target_dependencies(battery rclcpp example_interfaces my_robot_interfaces)

add_executable(led_panel src/led_panel.cpp)
ament_target_dependencies(led_panel rclcpp example_interfaces my_robot_interfaces)

install(TARGETS
  battery
  led_panel
  DESTINATION lib/${PROJECT_NAME}
)


ament_package()
