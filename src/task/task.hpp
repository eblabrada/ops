#ifndef OPS_TASK_HPP
#define OPS_TASK_HPP 1

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "../utils/json"
#include "../utils/systemfunc"

namespace ops::task {

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

  std::string get_code() const { return code; }

  void create_task_folder(const std::string &folder_path = ".") {
    std::string task_folder = folder_path + "/" + this->code;
    if (utils::create_directory(task_folder)) {
        std::cout << "Task folder created: " << task_folder << std::endl;
    } else {
        std::cerr << "Failed to create task folder: " << task_folder << std::endl;
        return;
    }
    
    json j;
    to_json(j);
    std::string json_file_path = task_folder + "/task.json";

    std::ofstream fout;

    fout.open(json_file_path);
    if (fout.is_open()) {
        std::cerr << "Task JSON file already exists and will be overwritten: " << json_file_path << std::endl;
    }

    fout << j.dump(4) << std::endl;
    fout.close();

    std::cout << "Task JSON file created: " << json_file_path << std::endl;   
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

  void create_task_folder(const std::string &folder_path = ".") {
    Task::create_task_folder(folder_path);
    std::string task_folder = folder_path + "/" + this->get_code();
    std::string json_file_path = task_folder + "/task.json";

    std::ifstream fin(json_file_path);
    if (!fin.is_open()) {
        std::cerr << "Failed to open task JSON file for reading: " << json_file_path << std::endl;
        return;
    }

    json j;
    fin >> j;
    fin.close();

    // add the interactive field
    j["interactive"] = true;

    std::ofstream fout(json_file_path);
    if (!fout.is_open()) {
        std::cerr << "Failed to open task JSON file for writing: " << json_file_path << std::endl;
        return;
    }

    fout << j.dump(4) << std::endl;
    fout.close();

    std::cout << "Task JSON file updated with interactive field: " << json_file_path << std::endl;
  }
};

}  // namespace ops::task

#endif  // OPS_TASK_HPP