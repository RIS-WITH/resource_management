#include "resource_management/plugins/EventsPluginTest.h"

#include <pluginlib/class_list_macros.h>

namespace resource_management
{

EventsPluginTest::EventsPluginTest() : _nh(new ros::NodeHandle())
{
  _subscriber = _nh->subscribe("resource_management/test", 10, &EventsPluginTest::callback, this);
}

void EventsPluginTest::callback(const std_msgs::String::ConstPtr& msg)
{
  if(_SpreadEvent)
    _SpreadEvent(msg->data);
}

PLUGINLIB_EXPORT_CLASS(EventsPluginTest, EventsInterface)

} //  namespace resource_management