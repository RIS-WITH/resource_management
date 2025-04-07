cmake_minimum_required(VERSION 3.8)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
project(resource_synchronizer_msgs)

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

find_package(resource_management_msgs REQUIRED)

# ###############################################
# # Declare ROS messages, services and actions ##
# ###############################################
if($ENV{ROS_VERSION} STREQUAL "1")
    # Generate messages in the 'msg' folder
    add_message_files(
        FILES
        MetaStateMachinesStatus.msg
        MetaStateMachineHeader.msg
        SubStateMachineHeader.msg
    )

    # Generate added messages and services with any dependencies listed here
    generate_messages(DEPENDENCIES std_msgs resource_management_msgs)

elseif($ENV{ROS_VERSION} STREQUAL "2")
    rosidl_generate_interfaces(resource_synchronizer_msgs
        "msg/MetaStateMachinesStatus.msg"
        "msg/MetaStateMachineHeader.msg"
        "msg/SubStateMachineHeader.msg"
        DEPENDENCIES builtin_interfaces std_msgs resource_management_msgs)
endif()

# ##################################
# #   ROS specific configuration  ##
# ##################################
if($ENV{ROS_VERSION} STREQUAL "1")
    catkin_package(CATKIN_DEPENDS message_runtime std_msgs resource_management_msgs)
elseif($ENV{ROS_VERSION} STREQUAL "2")
    ament_export_dependencies(rosidl_default_runtime resource_management_msgs)
endif()

# ##################################
# #            ROS2               ##
# ##################################
if($ENV{ROS_VERSION} STREQUAL "2")
    ament_package()
endif()