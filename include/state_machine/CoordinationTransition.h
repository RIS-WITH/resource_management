#ifndef COORDINATIONTRANSITION_H
#define COORDINATIONTRANSITION_H

#include <ros/ros.h>

#include <regex>
#include <chrono>
#include <string>

enum transtition_state_t
{
  transition_pass_on_event,
  transition_pass_on_duration,
  transition_timeout,
  transition_wait,
  transition_global_timeout,
  transition_none,
};

class CoordinationTransition
{
public:
  CoordinationTransition(ros::Duration duration, ros::Duration time_out, std::vector<std::string> regexs);

  void start();
  void reset();

  transtition_state_t evaluate();
  transtition_state_t evaluate(const std::string& event);

private:
  ros::Duration duration_;
  ros::Duration time_out_;
  std::vector<std::regex> regexs_;
  std::vector<bool> regexs_validation_;

  ros::Time start_;
};

#endif // COORDINATIONTRANSITION_H
