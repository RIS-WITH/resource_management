#ifndef STATESTORAGE_H
#define STATESTORAGE_H

#include <map>
#include <string>

#include "resource_management/message_storage/MessageAbstraction.h"
#include "resource_management/message_storage/MessagePriority.h"
#include "resource_management/state_machine/StateMachineState.h"

namespace resource_management {

  class StateStorage : public MessagePriority
  {
  public:
    StateStorage(uint32_t id = -1, compat::rm_ros::Duration time_out = compat::rm_ros::Duration(-1), compat::rm_ros::Time begin_dead_line = compat::rm_ros::Time(0));
    ~StateStorage();

    void addState(const std::string& id, bool partially_defined = false);
    void addTransition(const std::string& id, const std::string& id_next, StateMachineTransition& transition);
    void addData(const std::string& id, std::shared_ptr<MessageAbstraction> data);

    void setInitialState(const std::string& id);
    StateMachineState* getInitialState();
    std::shared_ptr<MessageAbstraction> getStateData(const std::string& id);

    compat::rm_ros::Duration getTimeout() { return time_out_; }
    compat::rm_ros::Time getDeadLine() { return begin_dead_line_; }
    uint32_t getId() { return id_; }
    bool isTooLate();

    StateMachineState* operator[](const std::string& id);

    void analyse();

  private:
    uint32_t id_;
    std::map<std::string, StateMachineState*> states_;
    std::map<std::string, std::shared_ptr<MessageAbstraction>> datas_;
    std::string initial_state_;

    compat::rm_ros::Duration time_out_;
    compat::rm_ros::Time begin_dead_line_;
  };

} // namespace resource_management

#endif // STATESTORAGE_H
