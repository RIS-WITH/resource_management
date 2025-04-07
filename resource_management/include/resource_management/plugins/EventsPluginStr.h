#ifndef EVENTSPLUGINTEST_H
#define EVENTSPLUGINTEST_H

#include <string>

#include "resource_management/compat/ros.h"
#include "resource_management/plugins/EventsInterface.h"

namespace resource_management {

  class EventsPluginStr : public EventsInterface
  {
  public:
    EventsPluginStr() {};
    ~EventsPluginStr() override = default;
    void init() override;

  private:
    compat::rm_ros::Subscriber<std_msgs_compat::String> subscriber_;

    void callback(const std_msgs_compat::String& msg);
  };

} //  namespace resource_management

#endif // EVENTSPLUGINTEST_H
