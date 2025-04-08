#include "resource_synchronizer/synchronizer/StateMachineSynchroHolder.h"

#include <cstddef>
#include <map>
#include <string>
#include <vector>

namespace resource_synchronizer {

  std::map<std::string, compat::rs_ros::Publisher<std_msgs_compat::String>> StateMachineSynchroHolder::publishers_;

  void StateMachineSynchroHolder::insert(const std::string& resource, const std::vector<std::string>& synchros)
  {
    for(const auto& synchro : synchros)
    {
      synchros_[synchro].push_back(resource);
      activations_[synchro].push_back(false);
    }
  }

  void StateMachineSynchroHolder::registerResource(const std::string& resource)
  {
    if(publishers_.find(resource) == publishers_.end())
      publishers_.emplace(resource, compat::rs_ros::Publisher<std_msgs_compat::String>("/" + resource + "/str_events", 100));
  }

  bool StateMachineSynchroHolder::activate(const std::string& synchro, const std::string& resource)
  {
    bool res = false;

    auto it = synchros_.find(synchro);
    if(it != synchros_.end())
    {
      res = true;

      for(size_t i = 0; i < it->second.size(); i++)
      {
        if(it->second[i] == resource)
          activations_[synchro][i] = true;

        res = res && activations_[synchro][i];
      }

      if(res)
      {
        std_msgs_compat::String msg;
        msg.data = "__synchro__" + it->first;
        for(auto& r : it->second)
          publishers_.at(r).publish(msg);

        reset(synchro);
      }
    }

    return res;
  }

  void StateMachineSynchroHolder::reset()
  {
    for(auto it : activations_)
      std::fill(it.second.begin(), it.second.end(), false);
  }

  void StateMachineSynchroHolder::reset(const std::string& synchro)
  {
    std::fill(activations_[synchro].begin(), activations_[synchro].end(), false);
  }

} // namespace resource_synchronizer
