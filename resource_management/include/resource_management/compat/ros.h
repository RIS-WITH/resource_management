#ifndef RM_COMPAT_ROS_H
#define RM_COMPAT_ROS_H

#if ROS_VERSION == 1
#include <ros/callback_queue.h>
#include <ros/ros.h>

// Commonly used built-in interfaces
#include <std_msgs/String.h>

// User-defined message interfaces
#include <resource_management_msgs/CompatDuration.h>
#include <resource_management_msgs/CompatTime.h>
#include <resource_management_msgs/EndCondition.h>
#include <resource_management_msgs/MessagePriority.h>
#include <resource_management_msgs/PrioritiesSetter.h>
#include <resource_management_msgs/StateMachineHeader.h>
#include <resource_management_msgs/StateMachineStateHeader.h>
#include <resource_management_msgs/StateMachineTransition.h>
#include <resource_management_msgs/StateMachinesStatus.h>

// User-defined service interfaces
#include <resource_management_msgs/StateMachinesCancel.h>

namespace std_msgs_compat = std_msgs;

#elif ROS_VERSION == 2
#include <rclcpp/rclcpp.hpp>

// Commonly used built-in interfaces
#include <std_msgs/msg/string.hpp>

// User-defined message interfaces
#include <resource_management_msgs/msg/compat_duration.hpp>
#include <resource_management_msgs/msg/compat_time.hpp>
#include <resource_management_msgs/msg/end_condition.hpp>
#include <resource_management_msgs/msg/message_priority.hpp>
#include <resource_management_msgs/msg/priorities_setter.hpp>
#include <resource_management_msgs/msg/state_machine_header.hpp>
#include <resource_management_msgs/msg/state_machine_state_header.hpp>
#include <resource_management_msgs/msg/state_machine_transition.hpp>
#include <resource_management_msgs/msg/state_machines_status.hpp>

// User-defined service interfaces
#include <resource_management_msgs/srv/state_machines_cancel.hpp>

namespace std_msgs_compat = std_msgs::msg;

namespace resource_management::msg {
}
namespace resource_management::srv {
}

#endif

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>

namespace resource_management::compat {

#if ROS_VERSION == 1

  using namespace ::resource_management;

  template<typename T>
  using RawRequestType = typename T::Request;

  template<typename T>
  using RawResponseType = typename T::Response;

  template<typename T>
  using RequestType = typename T::Request;

  template<typename T>
  using ResponseType = typename T::Response;

  template<typename T, typename Request_ = typename T::Request>
  inline auto makeRequest() { return Request_(); }

  template<typename T, typename Response_ = typename T::Response>
  inline auto makeResponse() { return Response_(); }

  // todo: RequestType, ResponseType

#elif ROS_VERSION == 2
  using namespace ::resource_management::msg;
  using namespace ::resource_management::srv;

  template<typename T>
  using RawRequestType = typename T::Request;

  template<typename T>
  using RawResponseType = typename T::Response;

  template<typename T>
  using RequestType = std::shared_ptr<typename T::Request>;

  template<typename T>
  using ResponseType = std::shared_ptr<typename T::Response>;

  template<typename T, typename Request_ = typename T::Request>
  inline auto makeRequest() { return std::make_shared<Request_>(); }

  template<typename T, typename Response_ = typename T::Response>
  inline auto makeResponse() { return std::make_shared<Response_>(); }

// template <typename T, typename Result_ = typename T::>
#endif

  namespace rm_ros {

#if ROS_VERSION == 1
    template<typename T>
    using ServiceWrapper = T;

    template<typename T>
    using MessageWrapper = typename T::ConstPtr;

    using Rate = ros::Rate;
    using RosTime = ros::Time;
    using RosDuration = ros::Duration;

    template<typename T>
    T* getServicePointer(T& service) { return &service; }

#elif ROS_VERSION == 2
    template<typename T>
    using ServiceWrapper = typename T::SharedPtr; // std::shared_ptr<T>;

    template<typename T>
    using MessageWrapper = typename T::ConstSharedPtr;

    using Rate = rclcpp::Rate;
    using RosTime = rclcpp::Time;
    using RosDuration = rclcpp::Duration;

    using namespace ::resource_management::msg;
    using namespace ::resource_management::srv;

    template<typename T>
    T& getServicePointer(T& service) { return service; }
#endif

    template<typename T>
    class Publisher;

    template<typename T>
    class Subscriber;

    template<typename T>
    class Service;

    template<typename T>
    class Client;

    class Time : public RosTime
    {
    public:
#if ROS_VERSION == 2
      static rcl_clock_type_t clock_type;
#endif

#if ROS_VERSION == 1
      Time(uint32_t sec, uint32_t nsec) : RosTime((int32_t)sec, (int32_t)nsec) {}
      explicit Time(int64_t t = 0) : RosTime(t) {}
#elif ROS_VERSION == 2
      Time(uint32_t sec, uint32_t nsec) : RosTime((int32_t)sec, (int32_t)nsec, Time::clock_type) {}
      explicit Time(int64_t t = 0) : RosTime(t, Time::clock_type) {}
#endif
      Time(const RosTime& time) : RosTime(time) {} // do not put it as explicit

      uint32_t seconds() const
      {
#if ROS_VERSION == 1
        return sec;
#elif ROS_VERSION == 2
        return (uint32_t)RosTime::seconds();
#endif
      }

      uint32_t nanoseconds() const
      {
#if ROS_VERSION == 1
        return nsec;
#elif ROS_VERSION == 2
        return RosTime::nanoseconds();
#endif
      }
    };

    class Duration : public RosDuration
    {
    public:
      Duration(uint32_t sec, uint32_t nsec) : RosDuration((int32_t)sec, (int32_t)nsec) {}

#if ROS_VERSION == 1
      explicit Duration(int64_t t) : RosDuration(t) {}
#elif ROS_VERSION == 2
      explicit Duration(int64_t t) : RosDuration(std::chrono::nanoseconds(t)) {}
#endif
      Duration(const RosDuration& time) : RosDuration(time)
      {} // do not put it as explicit

      uint32_t seconds() const
      {
#if ROS_VERSION == 1
        return sec;
#elif ROS_VERSION == 2
        return (uint32_t)RosDuration::seconds();
#endif
      }

      uint32_t nanoseconds() const
      {
#if ROS_VERSION == 1
        return nsec;
#elif ROS_VERSION == 2
        return RosDuration::nanoseconds();
#endif
      }
    };

    class Node
    {
    public:
      template<typename T>
      friend class Publisher;

      template<typename T>
      friend class Subscriber;

      template<typename T>
      friend class Service;

      template<typename T>
      friend class Client;

      Node(Node& other) = delete;
      Node(Node&& other) = delete;
      ~Node() = default;

      static Node& get();
      static bool ok();

      static void init(int argc, char** argv, const std::string& node_name);
      static void shutdown();

      static const std::string& getName();

      void spin();
      void spinOnce();

      Time currentTime();

#if ROS_VERSION == 2
      void joinThread();
#endif

    private:
      explicit Node(const std::string& node_name);

      const std::string name_;

#if ROS_VERSION == 1
      ros::NodeHandle handle_;
      ros::CallbackQueue callback_queue_;
#elif ROS_VERSION == 2
      rclcpp::Node::SharedPtr handle_;
      std::thread ros_thread_;
#endif

      bool running_;
    };

    template<typename T>
    class Publisher
    {
    public:
      Publisher(const std::string& topic_name, std::size_t queue_size)
      {
        auto& node = Node::get();

#if ROS_VERSION == 1
        handle_ = node.handle_.advertise<T>(topic_name, queue_size);
#elif ROS_VERSION == 2
        (void)queue_size;
        handle_ = node.handle_->create_publisher<T>(topic_name, 10);
#endif
      }

      void publish(const T& message)
      {
#if ROS_VERSION == 1
        handle_.publish(message);
#elif ROS_VERSION == 2
        handle_->publish(message);
#endif
      }

      size_t getNumSubscribers()
      {
#if ROS_VERSION == 1
        return handle_.getNumSubscribers();
#elif ROS_VERSION == 2
        return handle_->get_subscription_count();
#endif
      }

    private:
#if ROS_VERSION == 1
      ros::Publisher handle_;
#elif ROS_VERSION == 2
      typename rclcpp::Publisher<T>::SharedPtr handle_;
#endif
    };

    template<typename T>
    class Subscriber
    {
    public:
      template<typename Ta, typename Tb>
      Subscriber(const std::string& topic_name, std::size_t queue_size, Ta&& callback, Tb&& ptr)
      {
        auto& node = Node::get();

#if ROS_VERSION == 1
        handle_ = node.handle_.subscribe(topic_name, queue_size, callback, ptr);
#elif ROS_VERSION == 2
        (void)queue_size;
        handle_ = node.handle_->create_subscription<T>(topic_name, 10, std::bind(std::forward<Ta>(callback), ptr, std::placeholders::_1));
#endif
      }

      Subscriber() = default;

    private:
#if ROS_VERSION == 1
      ros::Subscriber handle_;
#elif ROS_VERSION == 2
      typename rclcpp::Subscription<T>::SharedPtr handle_;
#endif
    };

    template<typename T>
    class Service
    {
    public:
      template<typename Ta>
      Service(const std::string& service_name, Ta&& callback)
      {
        auto& node = Node::get();

#if ROS_VERSION == 1
        handle_ = node.handle_.advertiseService(service_name, callback);
#elif ROS_VERSION == 2
        handle_ = node.handle_->create_service<T>(service_name, [&](compat::rm_ros::ServiceWrapper<typename T::Request> req, compat::rm_ros::ServiceWrapper<typename T::Response> res) { callback(req, res); });
        // handle_ = node.handle_->create_service<T>(service_name, callback);
#endif
      }

      template<typename Ta, typename Tb>
      Service(const std::string& service_name, Ta&& callback, Tb&& ptr)
      {
        auto& node = Node::get();

#if ROS_VERSION == 1
        handle_ = node.handle_.advertiseService(service_name, callback, ptr);
#elif ROS_VERSION == 2
        handle_ = node.handle_->create_service<T>(service_name, [ptr, callback](compat::rm_ros::ServiceWrapper<typename T::Request> req, compat::rm_ros::ServiceWrapper<typename T::Response> res) { (ptr->*callback)(req, res); });
        // handle_ = node.handle_->create_service<T>(service_name, std::bind(std::forward<Ta>(callback), ptr, std::placeholders::_1, std::placeholders::_2));
#endif
      }

    private:
#if ROS_VERSION == 1
      ros::ServiceServer handle_;
#elif ROS_VERSION == 2
      typename rclcpp::Service<T>::SharedPtr handle_;
#endif
    };

    template<typename T>
    class Client
    {
    public:
      enum class Status_e
      {
        ros_status_successful,
        ros_status_successful_with_retry,
        ros_status_failure
      };

      explicit Client(const std::string& service_name) : name_(service_name)
      {
        auto& node = Node::get();

#if ROS_VERSION == 1
        handle_ = node.handle_.serviceClient<T>(service_name, true);
#elif ROS_VERSION == 2
        handle_ = node.handle_->create_client<T>(service_name);
#endif
      }

      Status_e call(const resource_management::compat::RequestType<T>& req, resource_management::compat::ResponseType<T>& res)
      {
        using namespace std::chrono_literals;
        auto status = Status_e::ros_status_failure;

#if ROS_VERSION == 1
        T srv;
        srv.request = req;
        if(!handle_.call(srv))
        {
          auto& node = Node::get();
          handle_ = node.handle_.serviceClient<T>(name_, true);
          if(handle_.call(srv))
          {
            status = Status_e::ros_status_successful_with_retry;
            res = srv.response;
          }
        }
        else
        {
          status = Status_e::ros_status_successful;
          res = srv.response;
        }

#elif ROS_VERSION == 2
        if(!handle_->wait_for_service(5s))
        {
          return status;
        }

        auto future = handle_->async_send_request(req);

        if(future.wait_for(5s) == std::future_status::ready)
        {
          status = Status_e::ros_status_successful;
          res = future.get();
        }
#endif
        return status;
      }

      bool wait(double timeout)
      {
#if ROS_VERSION == 1
        return handle_.waitForExistence(ros::Duration(timeout));
#elif ROS_VERSION == 2
        return handle_->wait_for_service(std::chrono::duration<double>(timeout));
#endif
      }

    private:
      std::string name_;
#if ROS_VERSION == 1
      ros::ServiceClient handle_;
#elif ROS_VERSION == 2
      typename rclcpp::Client<T>::SharedPtr handle_;
#endif
    };

  } // namespace rm_ros

} // namespace resource_management::compat

#endif // RM_COMPAT_ROS_H
