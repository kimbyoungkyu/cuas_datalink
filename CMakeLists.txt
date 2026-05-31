cmake_minimum_required(VERSION 3.8)
project(cuas_datalink)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(px4_msgs REQUIRED)
find_package(cuas_msgs REQUIRED)
find_package(nlohmann_json REQUIRED)

include_directories(include)

set(COMMON_DEPENDENCIES
  rclcpp
  std_msgs
  geometry_msgs
  nav_msgs
  sensor_msgs
  builtin_interfaces
  px4_msgs
  cuas_msgs
)

find_path(
  NATS_INCLUDE_DIR
  NAMES nats/nats.h
  PATHS
    /usr/local/include
    /usr/include
)

find_library(
  NATS_LIBRARY
  NAMES nats
  PATHS
    /usr/local/lib
    /usr/lib
    /usr/lib/x86_64-linux-gnu
)

if(NOT NATS_INCLUDE_DIR)
  message(FATAL_ERROR "NATS include directory not found")
endif()

if(NOT NATS_LIBRARY)
  message(FATAL_ERROR "NATS library not found")
endif()

message(STATUS "NATS include: ${NATS_INCLUDE_DIR}")
message(STATUS "NATS library: ${NATS_LIBRARY}")

# =========================================================
# c2_command_node
# =========================================================

add_executable(cuas_uplink_node
  src/cuas_uplink_node.cpp
)

ament_target_dependencies(cuas_uplink_node
  ${COMMON_DEPENDENCIES}
)

target_include_directories(cuas_uplink_node
  PRIVATE
    ${NATS_INCLUDE_DIR}
)

target_link_libraries(cuas_uplink_node
  ${NATS_LIBRARY}
  nlohmann_json::nlohmann_json
)

target_compile_definitions(cuas_uplink_node
  PRIVATE
    CUAS_DATALINK_HAS_NATS=1
)

# =========================================================
# interceptor_report_node
# =========================================================

add_executable(cuas_downlink_node
  src/cuas_downlink_node.cpp
)

ament_target_dependencies(cuas_downlink_node
  ${COMMON_DEPENDENCIES}
)

target_include_directories(cuas_downlink_node
  PRIVATE
    ${NATS_INCLUDE_DIR}
)

target_link_libraries(cuas_downlink_node
  ${NATS_LIBRARY}
  nlohmann_json::nlohmann_json
)

target_compile_definitions(cuas_downlink_node
  PRIVATE
    CUAS_DATALINK_HAS_NATS=1
)

# =========================================================
# Install
# =========================================================

install(TARGETS
  cuas_uplink_node
  cuas_downlink_node
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
  launch
  config
  DESTINATION share/${PROJECT_NAME}
)

ament_package()