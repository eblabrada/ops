#ifndef OPS_TASK_HPP
#define OPS_TASK_HPP 1

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "../utils/json"
#include "../utils/systemfunc"

namespace ops::task {

using json = nlohmann::json;
using ExecResult = ops::utils::ExecResult;

class Task {
 public:
  virtual ~Task() = default;

  Task() = default;
  Task(std::string code, std::string name)
      : code(std::move(code)), name(std::move(name)) {}

  void set_time_limit(double limit) { time_limit = limit; }
  void set_memory_limit(int limit) { memory_limit = limit; }

  const std::string& get_code() const { return code; }
  const std::string& get_name() const { return name; }

  double get_time_limit() const { return time_limit; }
  int get_memory_limit() const { return memory_limit; }

  virtual std::string type() const { return "batch"; }

  void load_common(const json& j) {
    j.at("code").get_to(code);
    j.at("name").get_to(name);
    j.at("time_limit").get_to(time_limit);
    j.at("memory_limit").get_to(memory_limit);
  }

  virtual void load_from_json(const json& j) { load_common(j); }

  virtual void to_json(json& j) const {
    j = json{{"type", type()},
             {"code", code},
             {"name", name},
             {"time_limit", time_limit},
             {"memory_limit", memory_limit}};
  }

  static std::unique_ptr<Task> from_json(const json& j);

  ExecResult create_task_folder(const std::string& folder_path = ".") {
    if (!this->task_folder.empty()) {
      return ExecResult{1, "task folder already exists"};
    }

    this->task_folder = folder_path + "/" + this->code;

    if (!utils::create_directory(task_folder).success()) {
      return ExecResult{2, "failed to create task folder"};
    }

    json j;
    this->to_json(j);
    std::string json_file_path = task_folder + "/task.json";

    std::ofstream fout(json_file_path);

    if (!fout.is_open()) {
      throw std::runtime_error("Failed to open task JSON file for writing: " +
                               json_file_path);
    }

    fout << j.dump(4) << std::endl;

    return ExecResult{0, "task folder created successfully"};
  }

 protected:
  std::string code;
  std::string name;
  double time_limit = 2.0;    // in seconds
  int memory_limit = 262144;  // in kilobytes
  std::string task_folder;
};

class InteractiveTask : public Task {
 public:
  using Task::Task;

  std::string type() const override { return "interactive"; }

  void load_from_json(const json& j) override {
    Task::load_common(j);
    if (j.at("type").get<std::string>() != "interactive") {
      throw std::runtime_error("JSON type mismatch: expected 'interactive'");
    }
  }

  void to_json(json& j) const override {
    Task::to_json(j);
    j["type"] = type();
  }
};

std::unique_ptr<Task> Task::from_json(const json& j) {
  const std::string t = j.value("type", "batch");

  std::unique_ptr<Task> task;
  if (t == "interactive") {
    task = std::make_unique<InteractiveTask>();
  } else {
    task = std::make_unique<Task>();
  }

  task->load_from_json(j);
  return task;
}

}  // namespace ops::task

#endif  // OPS_TASK_HPP