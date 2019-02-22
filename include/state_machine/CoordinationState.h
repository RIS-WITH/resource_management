#ifndef COORDINATIONSTATE_H
#define COORDINATIONSTATE_H

#include "state_machine/CoordinationTransition.h"

#include <string>
#include <vector>

class CoordinationSate
{
public:
  CoordinationSate(std::string id);

  void setTransition(CoordinationSate* next, CoordinationTransition tansition);

private:
  std::string id_;
  std::vector<CoordinationTransition> transitions_conditions_;
  std::vector<CoordinationSate*> transitions_next_state_;
};

#endif // COORDINATIONSTATE_H