#include "resource_management/state_machine/StateMachineState.h"

namespace resource_management {

StateMachineState::StateMachineState(std::string id)
{
  id_ = id;
}

void StateMachineState::setTransition(StateMachineState* next, StateMachineTransition tansition)
{
  transitions_conditions_.push_back(tansition);
  transitions_next_state_.push_back(next);
}

void StateMachineState::startState()
{
  for(auto& transition : transitions_conditions_)
    transition.start();
}

transtition_state_t StateMachineState::update(StateMachineState** current_state, const std::string& event)
{
  transtition_state_t transtition_state = transition_none;
  size_t next_index = -1;

  if(endState())
  {
    *current_state =  nullptr;
    return transtition_state;
  }

  for(size_t i = 0; i < transitions_conditions_.size(); i++)
  {
    if(event == "")
      transtition_state = transitions_conditions_[i].evaluate();
    else
      transtition_state = transitions_conditions_[i].evaluate(event);

    if((transtition_state == transition_pass_on_event) || (transtition_state == transition_pass_on_duration))
    {
      next_index = i;
      break;
    }
    else if(transtition_state == transition_timeout)
      break;
  }

  if((transtition_state == transition_pass_on_event) || (transtition_state == transition_pass_on_duration))
  {
    *current_state = transitions_next_state_[next_index];
    if((*current_state)->endState())
      *current_state =  nullptr;
  }
  else if(transtition_state == transition_timeout)
    *current_state =  nullptr;
  else if(transtition_state == transition_wait)
    *current_state = this;

  return transtition_state;
}

bool StateMachineState::endState()
{
  if(transitions_conditions_.size() == 0)
    return true;
  else
    return false;
}

} // namespace resource_management