#ifndef RESOURCE_SYNCHRONIZER_STATEMACHINE_H
#define RESOURCE_SYNCHRONIZER_STATEMACHINE_H

#include <string>

#include "resource_synchronizer/compat/ros.h"

namespace resource_synchronizer {

  template<typename T>
  class StateMachine
  {
  public:
    StateMachine(T state_machine_msg, compat::SubStateMachineHeader header, compat::MessagePriority importance)
    {
      state_machine_ = state_machine_msg;
      header_.initial_state = header.initial_state;
      header_.timeout = header.timeout;
      header_.begin_dead_line = header.begin_dead_line;
      header_.priority = importance;
    }

    bool isTooLate()
    {
      if(compat::rs_ros::Time(0) > compat::rs_ros::Time(header_.begin_dead_line.sec, header_.begin_dead_line.nanosec))
        return true;
      else
        return false;
    }

    compat::StateMachineHeader getHeaderMsg()
    {
      return header_;
    }

    T getStateMachineMsg()
    {
      return state_machine_;
    }

    T operator()()
    {
      return state_machine_;
    }

    int getPriority()
    {
      return header_.priority.value;
    }

  private:
    T state_machine_;
    compat::StateMachineHeader header_;
  };

} // namespace resource_synchronizer

#endif // RESOURCE_SYNCHRONIZER_STATEMACHINE_H
