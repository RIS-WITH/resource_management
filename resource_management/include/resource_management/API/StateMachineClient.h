#ifndef _RESOURCE_MANAGEMENT_INCLUDE_RESOURCE_MANAGEMENT_STATEMACHINESERVER_H_
#define _RESOURCE_MANAGEMENT_INCLUDE_RESOURCE_MANAGEMENT_STATEMACHINESERVER_H_

#include <functional>
#include <mutex>
#include <string>
#include <thread>

#include "resource_management/compat/ros.h"

namespace resource_management {

  struct stateMachineState_t
  {
    stateMachineState_t() {}
    stateMachineState_t(const std::string& state_name, const std::string& state_event)
    {
      state_name_ = state_name;
      state_event_ = state_event;
    }

    std::string toString()
    {
      return state_name_ + " : " + state_event_;
    }

    std::string state_name_;
    std::string state_event_;
  };

  template<class MessageType>
  class StateMachineClient
  {
  public:
    StateMachineClient(std::string name, bool synchronised = false, bool spin_thread = true);
    ~StateMachineClient();

    void waitForServer(compat::rm_ros::Duration timeout = compat::rm_ros::Duration(-1));
    void waitForServer(int32_t timeout);

    bool send(MessageType srv);
    bool waitForResult(compat::rm_ros::Duration timeout = compat::rm_ros::Duration(-1));
    stateMachineState_t getResult() { return state_; }
    bool cancel();

    void registerSatusCallback(std::function<void(stateMachineState_t)> status_callback) { status_callback_ = status_callback; }

  private:
    std::string name_;
    int id_;
    bool synchronised_;

    std::mutex terminate_mutex_;
    bool need_to_terminate_;
    std::thread* spin_thread_;

    std::string register_topic_name_;
    std::string cancel_topic_name_;
    std::string status_topic_name_;
    compat::rm_ros::Subscriber<compat::StateMachinesStatus> status_subscriber_;
    compat::rm_ros::Client<MessageType> client_;
    compat::rm_ros::Client<compat::StateMachinesCancel> cancel_client_;

    std::function<void(stateMachineState_t)> status_callback_;

    stateMachineState_t state_;

    void init(bool spin_thread);
    void spinThread();
    void statusCallback(const compat::rm_ros::MessageWrapper<compat::StateMachinesStatus>& msg);
  };

  template<class MessageType>
  StateMachineClient<MessageType>::StateMachineClient(std::string name, bool synchronised, bool spin_thread)
  {
    name_ = "/" + name;
    id_ = -1;
    synchronised_ = synchronised;

    init(spin_thread);

    register_topic_name_ = synchronised_ ? name_ + "/state_machines_register__" : name_ + "/state_machines_register";
    cancel_topic_name_ = synchronised_ ? name_ + "/state_machine_cancel__" : name_ + "/state_machine_cancel";
    status_topic_name_ = synchronised_ ? name_ + "/state_machine_status__" : name_ + "/state_machine_status";

    status_subscriber_ = compat::rm_ros::Subscriber<compat::StateMachinesStatus>(
      status_topic_name_,
      100,
      &StateMachineClient::statusCallback,
      this);

    client_ = compat::rm_ros::Client<MessageType>(register_topic_name_);
    cancel_client_ = compat::rm_ros::Client<compat::StateMachinesCancel>(cancel_topic_name_);
  }

  template<class MessageType>
  StateMachineClient<MessageType>::~StateMachineClient()
  {
    if(spin_thread_ != nullptr)
    {
      terminate_mutex_.lock();
      need_to_terminate_ = true;
      terminate_mutex_.unlock();
      spin_thread_->join();
      delete spin_thread_;
    }
  }

  template<class MessageType>
  void StateMachineClient<MessageType>::waitForServer(compat::rm_ros::Duration timeout)
  {
    client_.wait(timeout.seconds());
  }

  template<class MessageType>
  void StateMachineClient<MessageType>::waitForServer(int32_t timeout)
  {
    client_.wait(timeout);
  }

  template<class MessageType>
  bool StateMachineClient<MessageType>::send(MessageType srv)
  {
    if(client_.call(srv))
    {
      id_ = srv.response.id;
      state_.state_name_ = "_";

      return true;
    }
    else
      return false;
  }

  template<class MessageType>
  bool StateMachineClient<MessageType>::waitForResult(compat::rm_ros::Duration timeout)
  {
    compat::rm_ros::Time strat = compat::rm_ros::Time(0);
    bool end = false;

    while((end == false) && ((timeout == compat::rm_ros::Duration(-1)) || (compat::rm_ros::Time(0) - strat <= timeout)) && compat::rm_ros::Node::ok())
    {
      if(state_.state_name_.empty())
      {
        end = true;
        continue;
      }
    }

    return end;
  }

  template<class MessageType>
  bool StateMachineClient<MessageType>::cancel()
  {
    if(id_ == -1)
      return false;

    auto req = compat::makeRequest<compat::StateMachinesCancel>();
    auto res = compat::makeResponse<compat::StateMachinesCancel>();

    [this](auto&& req) {
      req->id = this->id_;
    }(compat::rm_ros::getServicePointer(req));

    using ResultTy = typename decltype(client_)::Status_e;

    if(cancel_client_.call(req, res) != ResultTy::ros_status_failure)
    {
      return [&](auto&& res) {
        return res->ack;
      };
    }
    else
      return false;
  }

  template<class MessageType>
  void StateMachineClient<MessageType>::init(bool spin_thread)
  {
    if(spin_thread)
    {
      std::cout << "Spinning up a thread for the StateMachineClient" << std::endl;
      need_to_terminate_ = false;
      spin_thread_ = new std::thread(std::bind(&StateMachineClient<MessageType>::spinThread, this));
    }
    else
    {
      spin_thread_ = nullptr;
    }
  }

  template<class MessageType>
  void StateMachineClient<MessageType>::spinThread()
  {
    while(compat::rm_ros::Node::ok())
    {
      terminate_mutex_.lock();
      if(need_to_terminate_)
        break;
      terminate_mutex_.unlock();
      compat::rm_ros::Node::get().spinOnce();
      usleep(10000);
    }
  }

  template<class MessageType>
  void StateMachineClient<MessageType>::statusCallback(const compat::rm_ros::MessageWrapper<compat::StateMachinesStatus>& msg)
  {
    if((int)msg->id == id_)
    {
      state_.state_name_ = msg->state_name;
      state_.state_event_ = msg->state_event;
      if(status_callback_)
        status_callback_(state_);
    }
  }

} // namespace resource_management

#endif // _RESOURCE_MANAGEMENT_INCLUDE_RESOURCE_MANAGEMENT_STATEMACHINESERVER_H_
