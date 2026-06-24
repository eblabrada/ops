#ifndef OPS_TASK_HPP
#define OPS_TASK_HPP 1

#include <string>
#include <vector>

#include "../utils/json"

namespace task {

using json = nlohmann::json;

class Task {
 public:
  Task(std::string code, std::string name, std::vector<std::string> authors)
      : code(std::move(code)),
        name(std::move(name)),
        authors(std::move(authors)) {}

  void add_author(const std::string& author) { authors.push_back(author); }

  void set_time_limit(double limit) { time_limit = limit; }
  void set_memory_limit(int limit) { memory_limit = limit; }

  void to_json(json& j) {
    j = json{{"code", code},
             {"name", name},
             {"authors", authors},
             {"time_limit", time_limit},
             {"memory_limit", memory_limit}};
  }

  void from_json(json& j) {
    j.at("code").get_to(this->code);
    j.at("name").get_to(this->name);
    j.at("authors").get_to(this->authors);
    j.at("time_limit").get_to(this->time_limit);
    j.at("memory_limit").get_to(this->memory_limit);
  }

 private:
  std::string code;
  std::string name;
  std::vector<std::string> authors;
  double time_limit = 2.0;    // in seconds
  int memory_limit = 262144;  // in kilobytes
};

class InteractiveTask : public Task {
 public:
  using Task::Task;

  void to_json(json& j) {
    Task::to_json(j);
    j["interactive"] = true;
  }

  void from_json(json& j) {
    Task::from_json(j);
    if (j.contains("interactive")) {
      if (!j["interactive"].get<bool>()) {
        throw std::runtime_error(
            "InteractiveTask must have interactive set to true");
      }
    } else {
      throw std::runtime_error("InteractiveTask must have interactive field");
    }
  }
};

}  // namespace task

#endif  // OPS_TASK_HPP