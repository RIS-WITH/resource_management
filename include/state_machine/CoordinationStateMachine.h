#ifndef COORDINATIONSTATEMACHINE_H
#define COORDINATIONSTATEMACHINE_H

#include "state_machine/EventStorage.h"

#include "state_machine/CoordinationTransition.h"
#include "state_machine/CoordinationState.h"

#include <ros/ros.h>

#include <string>
#include <mutex>

struct CoordinationInternalState_t
{
  CoordinationState* state_;
  transtition_state_t transition_state_;

  CoordinationInternalState_t()
  {
    state_ = nullptr;
    transition_state_ = transition_none;
  }

  CoordinationInternalState_t(CoordinationState* state, transtition_state_t transition_state)
  {
    state_ = state;
    transition_state_ = transition_state;
  }
};

class CoordinationStateMachine : public EventStorage
{
public:
  CoordinationStateMachine(ros::Duration time_out = ros::Duration(-1), ros::Time begin_dead_line = ros::Time(0), float rate = 100);

  void run();
  CoordinationInternalState_t getInternalState();
  std::string getCurrentStateName();

  void setInitialState(CoordinationState* state);
  void setPublicationFunction(void (*publishState)(CoordinationInternalState_t));

private:
  uint32_t us_sleep_time_;
  ros::Duration time_out_;
  ros::Time begin_dead_line_;

  void (*publishState_)(CoordinationInternalState_t);
  CoordinationInternalState_t internal_state_;
  std::mutex internal_state_mutex_;

  void runOnceNoEvent();
  void runOnceWithEvents(std::queue<std::string>& events);
};

#endif // COORDINATIONSTATEMACHINE_H
