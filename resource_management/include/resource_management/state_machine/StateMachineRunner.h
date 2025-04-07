#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <mutex>
#include <string>

#include "resource_management/compat/ros.h"
#include "resource_management/state_machine/EventStorage.h"
#include "resource_management/state_machine/StateMachineState.h"
#include "resource_management/state_machine/StateMachineTransition.h"

namespace resource_management {

  class StateMachineRunner : public EventStorage
  {
  public:
    StateMachineRunner(float rate = 100);

    void run();
    StateMachineInternalState_t getInternalState();
    std::string getCurrentStateName();
    bool isNewState();
    bool isWildcardState();
    bool runing();

    void setInitialState(StateMachineState* state, uint32_t state_machine_id = 0);
    void setPublicationFunction(std::function<void(StateMachineInternalState_t)> publishState);
    void setTimeout(compat::rm_ros::Duration time_out) { time_out_ = time_out; }
    void setDeadLine(compat::rm_ros::Time begin_dead_line) { begin_dead_line_ = begin_dead_line; }

  private:
    uint32_t us_sleep_time_;
    compat::rm_ros::Duration time_out_;
    compat::rm_ros::Time begin_dead_line_;

    std::function<void(StateMachineInternalState_t)> publishState_;
    StateMachineInternalState_t internal_state_;
    bool new_state_;
    std::mutex internal_state_mutex_;

    void runOnceNoEvent();
    void runOnceWithEvents(std::queue<std::string>& events);
  };

} // namespace resource_management

#endif // STATEMACHINE_H
