#include "resource_management/state_machine/StateMachineTransition.h"

#include <cstddef>
#include <iostream>
#include <regex>
#include <string>

#include "resource_management/compat/ros.h"

namespace resource_management {

  StateMachineTransition::StateMachineTransition(compat::rm_ros::Duration duration,
                                                 compat::rm_ros::Duration time_out,
                                                 const std::vector<std::string>& regexs) : duration_(duration),
                                                                                           time_out_(time_out)
  {
    for(const auto& regex : regexs)
    {
      regexs_.push_back(std::regex(regex));
      regexs_validation_.push_back(false);
      if(regex.find("__synchro__") == 0)
        synchro_names_.push_back(regex.substr(11));
    }
  }

  void StateMachineTransition::start()
  {
    start_ = compat::rm_ros::Node::get().currentTime();
  }

  void StateMachineTransition::reset()
  {
    start_ = compat::rm_ros::Time(0);
  }

  TranstitionState_e StateMachineTransition::evaluate()
  {
    compat::rm_ros::Time now = compat::rm_ros::Node::get().currentTime();

    if((time_out_ != compat::rm_ros::Duration(-1)) && (now - start_ >= time_out_))
      return transition_timeout;
    else if((duration_ != compat::rm_ros::Duration(-1)) && (now - start_ >= duration_))
      return transition_pass_on_duration;
    else
      return transition_wait;
  }

  TranstitionState_e StateMachineTransition::evaluate(const std::string& event)
  {
    compat::rm_ros::Time now = compat::rm_ros::Node::get().currentTime();

    if((time_out_ != compat::rm_ros::Duration(-1)) && (now - start_ >= time_out_))
      return transition_timeout;
    else if((duration_ != compat::rm_ros::Duration(-1)) && (now - start_ >= duration_))
      return transition_pass_on_duration;
    else
    {
      bool pass = true;

      if(regexs_validation_.size() == 0)
        pass = false;

      for(size_t i = 0; i < regexs_validation_.size(); i++)
      {
        std::smatch match;
        if(std::regex_match(event, match, regexs_[i]))
          regexs_validation_[i] = true;
        else if(regexs_validation_[i] == false)
          pass = false;
      }

      if(pass == true)
        return transition_pass_on_event;
      else
        return transition_wait;
    }
  }

  void StateMachineTransition::analyse(const std::string& state_name)
  {
    if(time_out_ == compat::rm_ros::Duration(0))
      std::cout << "\t\t" << compat::rm_ros::Node::get().getName() << "[WARNING] state " << state_name << ": transition timeout set to 0" << std::endl;

    if(duration_ == compat::rm_ros::Duration(0))
      std::cout << "\t\t" << compat::rm_ros::Node::get().getName() << "[WARNING] state " << state_name << ": transition duration set to 0" << std::endl;
  }

} // namespace resource_management
