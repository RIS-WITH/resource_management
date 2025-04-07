#ifndef MESSAGEPRIORITY_H
#define MESSAGEPRIORITY_H

#include "resource_management/message_storage/PriorityHolder.h"

namespace resource_management {

  class MessagePriority : public PriorityHolder<ImportancePriority_e>
  {
  public:
    MessagePriority() {};

    int operator*(FocusPriority_e other)
    {
      return (int)priorities_[(int)priority_][(int)other];
    }

  private:
  };

} // namespace resource_management

#endif // MESSAGEPRIORITY_H
