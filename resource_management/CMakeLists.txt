cmake_minimum_required(VERSION 3.8)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
project(resource_management)

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

find_package(pluginlib REQUIRED)
find_package(resource_management_msgs REQUIRED)
find_package(Threads)

# ##################################
# #   ROS specific configuration  ##
# ##################################
if($ENV{ROS_VERSION} STREQUAL "1")
  catkin_package(
    INCLUDE_DIRS include
    LIBRARIES message_lib state_machine_lib
    CATKIN_DEPENDS roscpp resource_management_msgs
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
    ament_target_dependencies(${TARGET} PUBLIC rclcpp pluginlib builtin_interfaces std_msgs resource_management_msgs)
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
add_ros_library(resource_management_compat
  src/compat/ros.cpp
)

add_ros_library(state_machine_lib
  src/state_machine/StateMachineTransition.cpp
  src/state_machine/StateMachineState.cpp
  src/state_machine/StateMachineRunner.cpp
  src/state_machine/EventStorage.cpp
  src/state_machine/StateStorage.cpp
  src/state_machine/StateMachinesStorage.cpp
)
target_link_libraries(state_machine_lib PUBLIC resource_management_compat)

add_ros_library(message_lib
  src/message_storage/ReactiveBuffer.cpp
  src/message_storage/ReactiveBufferStorage.cpp
)

add_ros_library(resource_management_plugin
  src/plugins/EventsPluginStr.cpp
)
target_link_libraries(resource_management_plugin PUBLIC state_machine_lib message_lib)

# ##################################
# #        C++ executables        ##
# ##################################
function(add_ros_executable TARGET)
  if(NOT TARGET)
    message(FATAL_ERROR "Expected the target name as first argument")
  endif()

  if(NOT ARGN)
    message(FATAL_ERROR "Expected source file list after target name")
  endif()

  add_executable(${TARGET} ${ARGN})

  if($ENV{ROS_VERSION} STREQUAL "1")
    target_include_directories(${TARGET} PUBLIC include PRIVATE ${catkin_INCLUDE_DIRS})
    target_link_libraries(${TARGET} PUBLIC ${catkin_LIBRARIES}) # PRIVATE has been removed fot ontoloGUI
  elseif($ENV{ROS_VERSION} STREQUAL "2")
    ament_target_dependencies(${TARGET} PUBLIC rclcpp pluginlib builtin_interfaces std_msgs resource_management_msgs)
    target_include_directories(${TARGET} PUBLIC include)
    target_link_libraries(${TARGET} PUBLIC ${cpp_typesupport_target})
  endif()

  set_target_properties(${TARGET} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
  target_compile_options(${TARGET} PUBLIC -DROS_VERSION=$ENV{ROS_VERSION})
  target_compile_options(${TARGET} PRIVATE -Wall -Wextra)
endfunction(add_ros_executable)

add_ros_executable(state_machine src/st_main.cpp)
target_link_libraries(state_machine PUBLIC state_machine_lib)

add_ros_executable(signals src/sig_main.cpp)
target_link_libraries(signals PUBLIC message_lib)

# ##################################
# #            Install            ##
# ##################################
if($ENV{ROS_VERSION} STREQUAL "1")
  install(TARGETS
    state_machine
    signals
    RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})

  install(TARGETS
    message_lib
    resource_management_plugin
    resource_management_compat
    state_machine_lib
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

  install(DIRECTORY manager_generator/
    DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/manager_generator
  )
elseif($ENV{ROS_VERSION} STREQUAL "2")
  install(TARGETS
    state_machine
    signals
    DESTINATION lib/${PROJECT_NAME})

  install(TARGETS
    message_lib
    resource_management_plugin
    resource_management_compat
    state_machine_lib
    EXPORT resource_management
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    DESTINATION lib/${PROJECT_NAME})

  install(DIRECTORY include/ DESTINATION include)

  ament_export_targets(resource_management)
endif()

# ##################################
# #            ROS2               ##
# ##################################
if($ENV{ROS_VERSION} STREQUAL "2")
  # pluginlib_export_plugin_description_file(project_name reasoners_plugins_ros2.xml)
  ament_export_include_directories(include)
  ament_package()
endif()