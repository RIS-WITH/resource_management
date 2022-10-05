#ifndef REACTIVEBUFFER_H
#define REACTIVEBUFFER_H

#include "resource_management/message_storage/BufferPriority.h"
#include "resource_management/message_storage/MessageAbstraction.h"

#include <string>
#include <memory>

namespace resource_management {

class ReactiveBuffer : public BufferPriority
{
public:
  explicit ReactiveBuffer(const std::string& name);

  const std::string& getName() const { return name_; }

  void setData(std::shared_ptr<MessageAbstraction> data);
  std::shared_ptr<MessageAbstraction> getData() const;
  std::shared_ptr<MessageAbstraction> operator()() const;

  void published() { has_been_published_ = true; }
  bool hasItBeenPublished() const { return has_been_published_; }

private:
  std::string name_;
  std::shared_ptr<MessageAbstraction> data_;
  bool has_been_published_;
};

} // namespace resource_management

#endif // REACTIVEBUFFER_H
