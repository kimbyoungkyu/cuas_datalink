cmake_minimum_required(VERSION 3.8)
project(cuas_datalink)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(CUAS_DATALINK_ENABLE_NATS "Build NATS bridge node" ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(px4_msgs REQUIRED)
find_package(cuas_msgs REQUIRED)

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

add_executable(c2_command_node src/c2_command_node.cpp)
ament_target_dependencies(c2_command_node ${COMMON_DEPENDENCIES})

add_executable(interceptor_status_node src/interceptor_status_node.cpp)
ament_target_dependencies(interceptor_status_node ${COMMON_DEPENDENCIES})

add_executable(datalink_router_node src/datalink_router_node.cpp)
ament_target_dependencies(datalink_router_node ${COMMON_DEPENDENCIES})

add_executable(heartbeat_node src/heartbeat_node.cpp)
ament_target_dependencies(heartbeat_node ${COMMON_DEPENDENCIES})

set(INSTALL_TARGETS
  c2_command_node
  interceptor_status_node
  datalink_router_node
  heartbeat_node
)

if(CUAS_DATALINK_ENABLE_NATS)
  find_path(NATS_INCLUDE_DIR nats/nats.h)
  find_library(NATS_LIBRARY nats)

  if(NATS_INCLUDE_DIR AND NATS_LIBRARY)
    message(STATUS "NATS found: ${NATS_LIBRARY}")
    add_executable(nats_bridge_node src/nats_bridge_node.cpp)
    target_include_directories(nats_bridge_node PRIVATE ${NATS_INCLUDE_DIR})
    target_link_libraries(nats_bridge_node ${NATS_LIBRARY})
    ament_target_dependencies(nats_bridge_node ${COMMON_DEPENDENCIES})
    list(APPEND INSTALL_TARGETS nats_bridge_node)
    target_compile_definitions(nats_bridge_node PRIVATE CUAS_DATALINK_HAS_NATS=1)
  else()
    message(WARNING "NATS C client was not found. nats_bridge_node will not be built. Install libnats-dev or build nats.c.")
  endif()
endif()

install(TARGETS
  ${INSTALL_TARGETS}
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
  launch
  config
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
