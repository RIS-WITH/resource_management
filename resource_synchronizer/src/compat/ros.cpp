#include "resource_synchronizer/compat/ros.h"

#include <string>

namespace resource_synchronizer::compat::rs_ros {

#if ROS_VERSION == 2
  rcl_clock_type_t Time::clock_type = rcl_clock_type_t::RCL_SYSTEM_TIME;
#endif

  std::string ros_node_name = "resource_synchronizer_ros";

  Node& Node::get()
  {
    static Node node_(ros_node_name);
    return node_;
  }

  bool Node::ok()
  {
#if ROS_VERSION == 1
    return ros::ok();
#elif ROS_VERSION == 2
    return rclcpp::ok();
#endif
  }

  void Node::init(int argc, char** argv, const std::string& node_name)
  {
    ros_node_name = node_name;

#if ROS_VERSION == 1
    ros::init(argc, argv, ros_node_name);
#elif ROS_VERSION == 2
    rclcpp::init(argc, argv);
    Time::clock_type = get().currentTime().get_clock_type();
#endif
  }

  void Node::shutdown()
  {
#if ROS_VERSION == 1
    ros::shutdown();
#elif ROS_VERSION == 2
    rclcpp::shutdown();
    get().joinThread();
#endif
  }

  const std::string& Node::getName()
  {
    return ros_node_name;
  }

  void Node::spin()
  {
#if ROS_VERSION == 1
    ros::spin();
#elif ROS_VERSION == 2
    // rclcpp::spin(handle_);
#endif
  }

  void Node::spinOnce()
  {
#if ROS_VERSION == 1
    ros::spinOnce();
#elif ROS_VERSION == 2
    // rclcpp::spin(handle_);
#endif
  }

  Time Node::currentTime()
  {
#if ROS_VERSION == 1
    return Time(ros::Time::now());
#elif ROS_VERSION == 2
    return Time(handle_->now());
#endif
  }

#if ROS_VERSION == 2
  void Node::joinThread()
  {
    ros_thread_.join();
  }
#endif

  Node::Node(const std::string& node_name) : name_(node_name),
#if ROS_VERSION == 2
                                             handle_(std::make_shared<rclcpp::Node>(node_name)),
#endif
                                             running_(true)
  {
    // todo: should we put something here?
#if ROS_VERSION == 2
    ros_thread_ = std::thread([this]() { rclcpp::spin(handle_); });
#endif
  }

} // namespace resource_synchronizer::compat::rs_ros
