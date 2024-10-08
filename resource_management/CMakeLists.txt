cmake_minimum_required (VERSION 2.8)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
project(resource_management)

add_compile_options(-std=c++14 -Wall -Wextra -pthread)

find_package(catkin REQUIRED COMPONENTS
    roscpp
    resource_management_msgs
    pluginlib
    )

catkin_package(
    INCLUDE_DIRS include
    LIBRARIES message_lib state_machine_lib
    CATKIN_DEPENDS roscpp resource_management_msgs
    )

find_package (Threads)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${INCLUDE_DIRS}
)

add_library(state_machine_lib
  src/state_machine/StateMachineTransition.cpp
  src/state_machine/StateMachineState.cpp
  src/state_machine/StateMachineRunner.cpp
  src/state_machine/EventStorage.cpp
  src/state_machine/StateStorage.cpp
  src/state_machine/StateMachinesStorage.cpp
)
add_dependencies(state_machine_lib ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

add_library(message_lib
  src/message_storage/ReactiveBuffer.cpp
  src/message_storage/ReactiveBufferStorage.cpp
)

add_library(resource_management_plugin
  src/plugins/EventsPluginStr.cpp
  )
target_link_libraries(resource_management_plugin state_machine_lib message_lib)

add_executable(state_machine src/st_main.cpp)
add_dependencies(state_machine ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(state_machine ${catkin_LIBRARIES} state_machine_lib)
target_link_libraries(state_machine ${CMAKE_THREAD_LIBS_INIT})

add_executable(signals src/sig_main.cpp)
target_link_libraries(signals message_lib)
target_link_libraries(signals ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS
  message_lib
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