#ifndef EVENTSINTERFACE_H
#define EVENTSINTERFACE_H

#include <functional>
#include <string>

#include "resource_management/compat/ros.h"

namespace resource_management {

  class EventsInterface
  {
  public:
    virtual ~EventsInterface() = default;

    virtual void init() {}

    void registerSpreading(std::function<void(const std::string&)> SpreadEvent)
    {
      spreadEvent_ = SpreadEvent;
    }

  protected:
    std::function<void(const std::string&)> spreadEvent_;
  };

} //  namespace resource_management

#endif // EVENTSINTERFACE_H
