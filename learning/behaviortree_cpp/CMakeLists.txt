cmake_minimum_required(VERSION 3.10)
project(bt_learning)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  behaviortree_cpp
  GIT_REPOSITORY https://github.com/BehaviorTree/BehaviorTree.CPP.git
  GIT_TAG master
)
FetchContent_MakeAvailable(behaviortree_cpp)

add_executable(bt_learning src/main.cpp)
target_link_libraries(bt_learning behaviortree_cpp)
