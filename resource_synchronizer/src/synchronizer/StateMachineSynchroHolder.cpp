#include "resource_synchronizer/synchronizer/StateMachineSynchroHolder.h"

#include "std_msgs/String.h"

namespace resource_synchronizer
{

ros::NodeHandlePtr StateMachineSynchroHolder::nh_;
std::map<std::string, ros::Publisher> StateMachineSynchroHolder::publishers_;

void StateMachineSynchroHolder::insert(const std::string& resource, const std::vector<std::string>& synchros)
{
  for(const auto& synchro : synchros)
  {
    synchros_[synchro].push_back(resource);
    activations_[synchro].push_back(false);
  }
}

void StateMachineSynchroHolder::registerResource(const std::string &resource)
{
  if(publishers_.find(resource) == publishers_.end())
    publishers_[resource] = nh_->advertise<std_msgs::String>("/" + resource + "/str_events", 100);
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
      std_msgs::String msg;
      msg.data = "__synchro__" + it->first;
      for(auto& r : it->second)
        publishers_[r].publish(msg);
      
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
