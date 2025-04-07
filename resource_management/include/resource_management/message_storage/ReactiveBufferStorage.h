#ifndef REACTIVEBUFFERSTORAGE_H
#define REACTIVEBUFFERSTORAGE_H

#include <map>
#include <string>
#include <vector>

#include "resource_management/message_storage/ReactiveBuffer.h"

namespace resource_management {

  class ReactiveBufferStorage
  {
  public:
    explicit ReactiveBufferStorage(const std::vector<std::string>& names);
    ~ReactiveBufferStorage() = default;

    void setPriority(const std::string& name, FocusPriority_e priority);
    std::shared_ptr<ReactiveBuffer> operator[](const std::string& name) const;
    double getHighestPriority();
    std::shared_ptr<ReactiveBuffer> getMorePriority();
    std::shared_ptr<MessageAbstraction> getMorePriorityData();

  private:
    std::map<std::string, std::shared_ptr<ReactiveBuffer>> buffers_;
    std::vector<std::string> buffers_names_;
  };

} // namespace resource_management

#endif // REACTIVEBUFFERSTORAGE_H
