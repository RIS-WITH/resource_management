#ifndef RESOURCE_MANAGEMENT_TOOLS_TOPIC_NAME_H
#define RESOURCE_MANAGEMENT_TOOLS_TOPIC_NAME_H

#include <regex>
#include <string>

namespace resource_management::tools {

  template<class MessageType>
  std::string topicName(const std::string& reactive_buffer_name, const std::string& ns = "")
  {
    std::string name = ns;
    if(!name.empty() && name[name.size() - 1] != '/')
      name += '/';
    name += reactive_buffer_name + "/";
    std::regex regex_name("^N\\d+(.*)\\d+(.*)_ISaIvEEE$");
    std::smatch match;
    std::string type_id = typeid(MessageType).name();
    if(std::regex_match(type_id, match, regex_name))
      name += match[1].str() + "_" + match[2].str();
    else
      name += typeid(MessageType).name();

    return name;
  }

} // namespace resource_management::tools

#endif // RESOURCE_MANAGEMENT_TOOLS_TOPIC_NAME_H
