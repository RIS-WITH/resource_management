#ifndef _RESOURCE_MANAGEMENT_INCLUDE_RESOURCE_MANAGEMENT_COORDINATIONSERVERBASE_H_
#define _RESOURCE_MANAGEMENT_INCLUDE_RESOURCE_MANAGEMENT_COORDINATIONSERVERBASE_H_

#include <functional>

#include <ros/ros.h>

#include "resource_management_msgs/StateMachinesStatus.h"
#include "resource_management_msgs/StateMachinesCancel.h"

namespace resource_management
{

template<class MessageType>
class CoordinationServerBase
{
public:
  CoordinationServerBase(ros::NodeHandlePtr nh, syd::string name)
  {
    _nh = nh;
    _name = name;
    _id = -1;
  }

  bool sendCoordinationSignal(MessageType srv)
  {
    ros::ServiceClient client = n.serviceClient<MessageType>(_name + "/coordination_signal_register");
    if(client.call(srv))
      return true;
    else
      return false;
  }

  bool cancel()
  {
    if(id_ == -1)
      return false;

    ros::ServiceClient client = n.serviceClient<resource_management_msgs::StateMachinesCancel>(_name + "/coordination_signal_cancel");
    resource_management_msgs::StateMachinesCancel srv;
    srv.request.id = _id;
    _id = -1;
    if(client.call(srv))
      return srv.response.ack;
    else
      return false;
  }
private:
  ros::NodeHandlePtr _nh;
  std::string _name;
  int _id;
};

}

#endif // _RESOURCE_MANAGEMENT_INCLUDE_RESOURCE_MANAGEMENT_COORDINATIONSERVERBASE_H_
