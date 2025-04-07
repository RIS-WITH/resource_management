#include "resource_management/plugins/EventsPluginStr.h"

#include <pluginlib/class_list_macros.hpp>

namespace resource_management {

  void EventsPluginStr::init()
  {
    subscriber_ = compat::rm_ros::Subscriber<std_msgs_compat::String>(
      "str_events",
      100,
      &EventsPluginStr::callback,
      this);
  }

  void EventsPluginStr::callback(const std_msgs_compat::String& msg)
  {
    if(spreadEvent_)
      spreadEvent_(msg.data);
  }

} //  namespace resource_management

PLUGINLIB_EXPORT_CLASS(resource_management::EventsPluginStr, resource_management::EventsInterface)
