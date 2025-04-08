#ifndef RESOURCE_SYNCHRONIZER_STATEMACHINESYNCHROHOLDER
#define RESOURCE_SYNCHRONIZER_STATEMACHINESYNCHROHOLDER

#include <map>
#include <string>
#include <vector>

#include "resource_synchronizer/compat/ros.h"

namespace resource_synchronizer {

  class StateMachineSynchroHolder
  {
  public:
    void insert(const std::string& resource, const std::vector<std::string>& synchros);

    bool activate(const std::string& synchro, const std::string& resource);
    void reset();
    void reset(const std::string& synchro);
    static void registerResource(const std::string& resource);

  private:
    std::map<std::string, std::vector<std::string>> synchros_;
    std::map<std::string, std::vector<bool>> activations_;

    static std::map<std::string, compat::rs_ros::Publisher<std_msgs_compat::String>> publishers_;
  };

} // namespace resource_synchronizer

#endif // RESOURCE_SYNCHRONIZER_STATEMACHINESYNCHROHOLDER
