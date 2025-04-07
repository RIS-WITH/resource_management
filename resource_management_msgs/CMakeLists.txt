cmake_minimum_required(VERSION 3.8)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
project(resource_management_msgs)

if($ENV{ROS_VERSION} STREQUAL "1")
  find_package(catkin REQUIRED COMPONENTS
    std_msgs
    message_generation
  )
elseif($ENV{ROS_VERSION} STREQUAL "2")
  find_package(ament_cmake REQUIRED)
  find_package(rosidl_default_generators REQUIRED)
  find_package(ament_cmake_python REQUIRED)
  find_package(std_msgs REQUIRED)
  find_package(builtin_interfaces REQUIRED)
endif()

# ###############################################
# # Declare ROS messages, services and actions ##
# ###############################################
if($ENV{ROS_VERSION} STREQUAL "1")
  # Generate messages in the 'msg' folder
  add_message_files(
    FILES
    CompatDuration.msg
    CompatTime.msg
    EndCondition.msg
    MessagePriority.msg
    PrioritiesSetter.msg
    StateMachineHeader.msg
    StateMachinesStatus.msg
    StateMachineStateHeader.msg
    StateMachineTransition.msg
  )

  # Generate services in the 'srv' folder
  add_service_files(
    FILES
    StateMachinesCancel.srv
  )

  # Generate added messages and services with any dependencies listed here
  generate_messages(DEPENDENCIES std_msgs)

elseif($ENV{ROS_VERSION} STREQUAL "2")
  rosidl_generate_interfaces(resource_management_msgs
    "msg/CompatDuration.msg"
    "msg/CompatTime.msg"
    "msg/EndCondition.msg"
    "msg/MessagePriority.msg"
    "msg/PrioritiesSetter.msg"
    "msg/StateMachineHeader.msg"
    "msg/StateMachinesStatus.msg"
    "msg/StateMachineStateHeader.msg"
    "msg/StateMachineTransition.msg"
    "srv/StateMachinesCancel.srv"
    DEPENDENCIES builtin_interfaces std_msgs)
endif()

# ##################################
# #   ROS specific configuration  ##
# ##################################
if($ENV{ROS_VERSION} STREQUAL "1")
  catkin_package(CATKIN_DEPENDS message_runtime std_msgs)
elseif($ENV{ROS_VERSION} STREQUAL "2")
  ament_export_dependencies(rosidl_default_runtime)
endif()

# ##################################
# #            ROS2               ##
# ##################################
if($ENV{ROS_VERSION} STREQUAL "2")
  ament_package()
endif()