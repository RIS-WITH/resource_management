#ifndef STATEMACHINESSTORAGE_H
#define STATEMACHINESSTORAGE_H

#include "resource_management/state_machine/StateStorage.h"

#include <vector>
#include <mutex>

namespace resource_management {

class StateMachinesStorage
{
public:
  StateMachinesStorage() { unpoppable_ = false; }
  bool empty();
  size_t size() const { return states_storage_.size(); }
  void push(std::shared_ptr<StateStorage>& state_storage);
  std::shared_ptr<StateStorage> pop(double priority = -100);
  bool poppable(double priority = -100);
  void setUnpoppable() { unpoppable_ = true; }
  bool remove(uint32_t id);

  void setPublicationFunction(std::function<void(StateMachineInternalState_t)> publishState);
  void clean();

private:
  std::vector<std::shared_ptr<StateStorage> > states_storage_;
  std::mutex mutex_;
  bool unpoppable_;

  std::function<void(StateMachineInternalState_t)> publishState_;
};

} // namespace resource_management

#endif //STATEMACHINESSTORAGE_H
