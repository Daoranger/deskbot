cmake_minimum_required(VERSION 3.8)
project(brobot_behavior)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(behaviortree_cpp REQUIRED)
find_package(ament_index_cpp REQUIRED)

add_executable(brobot_behavior_tree
  src/brobot_behavior_tree.cpp
)

ament_target_dependencies(brobot_behavior_tree
  rclcpp
  geometry_msgs
  behaviortree_cpp
  ament_index_cpp
)

install(TARGETS
  brobot_behavior_tree
  DESTINATION lib/${PROJECT_NAME}
)

install(
  DIRECTORY behavior_trees
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
