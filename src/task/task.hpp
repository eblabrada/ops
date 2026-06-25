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

class Task {
 public:
  Task(std::string code, std::string name)
      : code(std::move(code)), name(std::move(name)) {}

  void set_time_limit(double limit) { time_limit = limit; }
  void set_memory_limit(int limit) { memory_limit = limit; }

  void to_json(json& j) {
    j = json{{"code", code},
             {"name", name},
             {"time_limit", time_limit},
             {"memory_limit", memory_limit}};
  }

  void from_json(json& j) {
    j.at("code").get_to(this->code);
    j.at("name").get_to(this->name);
    j.at("time_limit").get_to(this->time_limit);
    j.at("memory_limit").get_to(this->memory_limit);
  }

  std::string get_code() const { return code; }

  bool create_task_folder(const std::string& folder_path = ".") {
    if (!this->task_folder.empty()) {
      std::cerr << "Task folder already exists: " << this->task_folder
                << std::endl;
      return false;
    }

    this->task_folder = folder_path + "/" + this->code;
    if (utils::create_directory(task_folder)) {
      std::cout << "Task folder created: " << task_folder << std::endl;
    } else {
      std::cerr << "Failed to create task folder: " << task_folder << std::endl;
      return false;
    }

    json j;
    to_json(j);
    std::string json_file_path = task_folder + "/task.json";

    std::ofstream fout;

    fout.open(json_file_path);
    if (!fout.is_open()) {
      throw std::runtime_error("Failed to open task JSON file for writing: " +
                               json_file_path);
      return false;
    }

    fout << j.dump(4) << std::endl;
    fout.close();

    std::cout << "Task JSON file created: " << json_file_path << std::endl;
    return true;
  }

 private:
  std::string code;
  std::string name;
  double time_limit = 2.0;    // in seconds
  int memory_limit = 262144;  // in kilobytes
  std::string task_folder;
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

  bool create_task_folder(const std::string& folder_path = ".") {
    if (!Task::create_task_folder(folder_path)) {
      return false;
    }

    std::string task_folder = folder_path + "/" + this->get_code();
    std::string json_file_path = task_folder + "/task.json";

    std::ifstream fin(json_file_path);
    if (!fin.is_open()) {
      throw std::runtime_error("Failed to open task JSON file for reading: " +
                               json_file_path);
      return false;
    }

    json j;
    fin >> j;
    fin.close();

    // add the interactive field
    j["interactive"] = true;

    std::ofstream fout(json_file_path);
    if (!fout.is_open()) {
      throw std::runtime_error("Failed to open task JSON file for writing: " +
                               json_file_path);
      return false;
    }

    fout << j.dump(4) << std::endl;
    fout.close();

    std::cout << "Task JSON file updated with interactive field: "
              << json_file_path << std::endl;
    return true;
  }
};

}  // namespace ops::task

#endif  // OPS_TASK_HPP