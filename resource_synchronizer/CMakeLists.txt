cmake_minimum_required(VERSION 3.8)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
project(resource_synchronizer)

# ###############################################
# #          Find macros and libraries         ##
# ###############################################
if($ENV{ROS_VERSION} STREQUAL "1")
  find_package(catkin REQUIRED COMPONENTS
    roscpp
  )

elseif($ENV{ROS_VERSION} STREQUAL "2")
  find_package(ament_cmake REQUIRED)
  find_package(rclcpp REQUIRED)
endif()

find_package(resource_management_msgs REQUIRED)
find_package(resource_management REQUIRED)
find_package(resource_synchronizer_msgs REQUIRED)

# ##################################
# #   ROS specific configuration  ##
# ##################################
if($ENV{ROS_VERSION} STREQUAL "1")
  catkin_package(
    INCLUDE_DIRS include
    LIBRARIES resource_synchronizer_lib synchronizer_lib resource_synchronizer_compat
    CATKIN_DEPENDS roscpp resource_management_msgs resource_synchronizer_msgs resource_management
  )
elseif($ENV{ROS_VERSION} STREQUAL "2")
  ament_export_dependencies(rosidl_default_runtime pluginlib)
endif()

# ##################################
# #    ROS dependent libraries    ##
# ##################################
function(add_ros_library TARGET)
  if(NOT TARGET)
    message(FATAL_ERROR "Expected the target name as first argument")
  endif()

  if(NOT ARGN)
    message(FATAL_ERROR "Expected source file list after target name")
  endif()

  add_library(${TARGET} ${ARGN})

  if($ENV{ROS_VERSION} STREQUAL "1")
    add_dependencies(${TARGET} ${catkin_EXPORTED_TARGETS})
    target_include_directories(${TARGET} PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:include>
      ${catkin_INCLUDE_DIRS})
    target_link_libraries(${TARGET} PRIVATE ${catkin_LIBRARIES})
  elseif($ENV{ROS_VERSION} STREQUAL "2")
    ament_target_dependencies(${TARGET} PUBLIC rclcpp pluginlib builtin_interfaces std_msgs
      resource_management_msgs
      resource_synchronizer_msgs
      resource_management)
    target_include_directories(${TARGET} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:include>)
    target_link_libraries(${TARGET} PUBLIC ${cpp_typesupport_target})
    ament_export_libraries(${TARGET})
  endif()

  set_target_properties(${TARGET} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
  target_compile_options(${TARGET} PRIVATE $<$<CXX_COMPILER_ID:Clang>:-frelaxed-template-template-args>)
  target_compile_options(${TARGET} PUBLIC -DROS_VERSION=$ENV{ROS_VERSION})
  target_compile_options(${TARGET} PRIVATE -Wall -Wextra)
endfunction(add_ros_library)

# # Compatibility
add_ros_library(resource_synchronizer_compat
  src/compat/ros.cpp
)

add_ros_library(synchronizer_lib
  src/synchronizer/StateMachineSynchroHolder.cpp
  src/synchronizer/StateMachinesSynchronizer.cpp
)

add_ros_library(resource_synchronizer_lib
  src/StateMachinesManager.cpp
)

# ##################################
# #            Install            ##
# ##################################
if($ENV{ROS_VERSION} STREQUAL "1")
  install(TARGETS
    resource_synchronizer_compat
    resource_synchronizer_lib
    synchronizer_lib
    ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
    LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
    RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

  install(DIRECTORY include/${PROJECT_NAME}/
    DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
    FILES_MATCHING PATTERN "*.h"
    PATTERN ".svn" EXCLUDE
  )

  install(DIRECTORY synchronizer_generator/
    DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/synchronizer_generator
  )
elseif($ENV{ROS_VERSION} STREQUAL "2")
  install(TARGETS
    resource_synchronizer_compat
    resource_synchronizer_lib
    synchronizer_lib
    EXPORT resource_synchronizer
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    DESTINATION lib/${PROJECT_NAME})

  install(DIRECTORY include/ DESTINATION include)

  install(DIRECTORY synchronizer_generator DESTINATION share/${PROJECT_NAME}/)

  ament_export_targets(resource_synchronizer)
endif()

# ##################################
# #            ROS2               ##
# ##################################
if($ENV{ROS_VERSION} STREQUAL "2")
  ament_export_include_directories(include)
  ament_package()
endif()