#ifndef RESOURCE_SYNCHRONIZER_STATEMACHINESMANAGER_H
#define RESOURCE_SYNCHRONIZER_STATEMACHINESMANAGER_H

#include <atomic>
#include <map>
#include <mutex>
#include <vector>

#include "resource_synchronizer/StateMachinesHolder.h"
#include "resource_synchronizer/compat/ros.h"

namespace resource_synchronizer {

  class StateMachinesManager
  {
  public:
    StateMachinesManager() : run_(false) {}
    ~StateMachinesManager() {}

    void registerHolder(StateMachinesHolderBase* holder);
    void registerSatusCallback(std::function<void(SubStateMachineStatus)> status_callback) { status_callback_ = status_callback; }

    void insert(int id, compat::MetaStateMachineHeader header);
    void cancel(int meta_sm_id);

    void run();
    void stop();

    void halt() { mutex_.lock(); }
    void realease() { mutex_.unlock(); }

  private:
    std::vector<StateMachinesHolderBase*> state_machines_holders_;
    std::map<int, compat::MetaStateMachineHeader> headers_;
    std::map<int, compat::rs_ros::Time> sm_start_time_;

    std::vector<int> running_ids_;
    std::atomic<bool> run_;
    std::mutex mutex_;

    std::vector<bool> done_;
    std::vector<std::vector<int>> ids_;
    std::vector<bool> preempt_;

    std::function<void(SubStateMachineStatus)> status_callback_;

    void init();
    void reinit();

    bool isDone();

    void applyConstraints();
    void clean();
    void cleanPreempted();

    void selectSm(int id);
    void removeSmFromSelection(int id);
    void removeSm(int id);
    bool isSelectable(size_t owner, int id);

    void publishTimeoutStatus(int id);
    void publishBDLStatus(int id);
    void publishPreemptStatus(int id);
  };

} // namespace resource_synchronizer

#endif // RESOURCE_SYNCHRONIZER_STATEMACHINESMANAGER_H
