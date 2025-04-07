#ifndef STATEMACHINETRANSITION_H
#define STATEMACHINETRANSITION_H

#include <chrono>
#include <regex>
#include <string>
#include <vector>

#include "resource_management/compat/ros.h"

namespace resource_management {

  enum TranstitionState_e
  {
    transition_pass_on_event,
    transition_pass_on_duration,
    transition_timeout,
    transition_wait,
    transition_wait_synchro,
    transition_global_timeout,
    transition_preampt,
    transition_dead_line,
    transition_none,
  };

  class StateMachineTransition
  {
  public:
    StateMachineTransition(compat::rm_ros::Duration duration, compat::rm_ros::Duration time_out, const std::vector<std::string>& regexs);

    void start();
    void reset();

    TranstitionState_e evaluate();
    TranstitionState_e evaluate(const std::string& event);

    std::vector<std::string> getSynchroNames() const { return synchro_names_; }

    void analyse(const std::string& state_name);

  private:
    compat::rm_ros::Duration duration_;
    compat::rm_ros::Duration time_out_;
    std::vector<std::regex> regexs_;
    std::vector<bool> regexs_validation_;
    std::vector<std::string> synchro_names_;

    compat::rm_ros::Time start_;
  };

} // namespace resource_management

#endif // STATEMACHINETRANSITION_H
