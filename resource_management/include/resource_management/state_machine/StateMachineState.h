#ifndef STATEMACHINESTATE_H
#define STATEMACHINESTATE_H

#include <string>
#include <vector>

#include "resource_management/state_machine/StateMachineTransition.h"

namespace resource_management {

  class StateMachineState;

  struct StateMachineInternalState_t
  {
    uint32_t state_machine_id;
    StateMachineState* state_;
    TranstitionState_e transition_state_;
    std::string synchro_name_;

    StateMachineInternalState_t() : state_machine_id(-1),
                                    state_(nullptr),
                                    transition_state_(transition_none)
    {}

    StateMachineInternalState_t(StateMachineState* state, TranstitionState_e transition_state) : state_machine_id(-1),
                                                                                                 state_(state),
                                                                                                 transition_state_(transition_state)
    {}
  };

  class StateMachineState
  {
  public:
    explicit StateMachineState(const std::string& id, bool partially_defined = false);

    void setTransition(StateMachineState* next, const StateMachineTransition& tansition);
    std::string getName() const { return id_; }

    void startState();
    TranstitionState_e update(StateMachineState** current_state, const std::string& event = "");
    bool endState() const;

    std::vector<std::string> getSynchroNames() const { return synchro_names_; }

    bool isPartiallyDefined() const { return partially_defined_; }
    void setAsDefined() { partially_defined_ = false; }
    void analyse();

  private:
    std::string id_;
    bool partially_defined_;
    std::vector<StateMachineTransition> transitions_conditions_;
    std::vector<StateMachineState*> transitions_next_state_;
    std::vector<std::string> synchro_names_;
  };

} // namespace resource_management

#endif // STATEMACHINESTATE_H
