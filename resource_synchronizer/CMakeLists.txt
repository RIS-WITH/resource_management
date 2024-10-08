cmake_minimum_required (VERSION 2.8)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
project (resource_synchronizer)

add_compile_options(-std=c++14 -Wall -Wextra -pthread)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  resource_management_msgs
  resource_management
  resource_synchronizer_msgs
  )

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES resource_synchronizer_lib synchronizer_lib
  CATKIN_DEPENDS roscpp resource_management_msgs resource_synchronizer_msgs resource_management
  )

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${INCLUDE_DIRS}
)

add_library(synchronizer_lib
  src/synchronizer/StateMachineSynchroHolder.cpp
  src/synchronizer/StateMachinesSynchronizer.cpp
)
add_dependencies(synchronizer_lib ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

add_library(resource_synchronizer_lib
  src/StateMachinesManager.cpp
)
add_dependencies(resource_synchronizer_lib ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

install(TARGETS
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