#include <fstream>
#include <iostream>

#include "all"

using json = nlohmann::json;

template <typename It>
std::string join(It first, It last, const std::string& sep = " ") {
  std::string result;

  if (first == last) return result;

  result += *first++;
  while (first != last) {
    result += sep;
    result += *first++;
  }

  return result;
}

void load_state(json& ops) {
  if (!ops::utils::file_exists(".ops").success()) {
    ops = json{{"current_olympiad", ""},
               {"current_task", ""},
               {"olympiads", json::array()}};

    std::ofstream fout(".ops");
    fout << ops.dump(4) << std::endl;
  } else {
    std::ifstream fin(".ops");
    fin >> ops;
  }
}

void save_state(const json& ops) {
  std::ofstream fout(".ops");
  fout << ops.dump(4) << std::endl;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "usage: " << argv[0] << " <command> [args...]" << std::endl;
    return 1;
  }

  json ops;
  load_state(ops);

  std::string entity = argv[1];

  if (entity == "olympiad") {
    if (argc < 3) {
      std::cerr << "usage: " << argv[0] << " olympiad <action>" << std::endl;
      return 1;
    }

    std::string action = argv[2];

    if (action == "list") {
      for (auto& olympiad : ops["olympiads"]) {
        bool current = olympiad["code"] == ops["current_olympiad"];

        std::cout << (current ? "* " : "  ")
                  << olympiad["code"].get<std::string>() << std::endl;
      }

      return 0;
    } else if (action == "create") {
      if (argc < 4) {
        std::cerr << "usage: " << argv[0] << " olympiad create <code>"
                  << std::endl;
        return 1;
      }

      std::string code = argv[3];

      for (auto& olympiad : ops["olympiads"]) {
        if (olympiad["code"] == code) {
          std::cerr << argv[0] << ": olympiad '" << code << "' already exists"
                    << std::endl;
          return 1;
        }
      }

      ops["olympiads"].push_back({{"code", code}, {"tasks", json::array()}});
      ops["current_olympiad"] = code;

      save_state(ops);

      std::cout << argv[0] << ": olympiad '" << code << "' created successfully"
                << std::endl;
      return 0;
    } else if (action == "use") {
      if (argc < 4) {
        std::cerr << "usage: " << argv[0] << " olympiad use <code>"
                  << std::endl;
        return 1;
      }

      std::string code = argv[3];

      for (auto& olympiad : ops["olympiads"]) {
        if (olympiad["code"] == code) {
          ops["current_olympiad"] = code;
          ops["current_task"] = "";
          save_state(ops);

          std::cout << argv[0] << ": using olympiad '" << code << "'"
                    << std::endl;
          return 0;
        }
      }

      ops["current_olympiad"] = "";
      ops["current_task"] = "";
      save_state(ops);

      std::cerr << argv[0] << ": olympiad '" << code << "' does not exist"
                << std::endl;
      return 1;
    } else {
      std::cerr << argv[0] << ": olympiad '" << action
                << "' is not a valid action. See '" << argv[0] << " help'."
                << std::endl;
      return 1;
    }
  }

  if (entity == "task") {
    if (ops["current_olympiad"].get<std::string>().empty()) {
      std::cerr << argv[0] << ": no olympiad is currently in use. Use '"
                << argv[0] << " olympiad use <code>' to select an olympiad."
                << std::endl;
      return 1;
    }

    if (argc < 3) {
      std::cerr << "usage: " << argv[0] << " task <action>" << std::endl;
      return 1;
    }

    std::string action = argv[2];

    if (action == "create") {
      if (argc < 5) {
        std::cerr << "usage: " << argv[0] << " task create <code> <name>"
                  << std::endl;
        return 1;
      }

      std::string code = argv[3];
      std::string name = join(argv + 4, argv + argc);

      auto olympiad_code = ops["current_olympiad"].get<std::string>();

      for (auto& olympiad : ops["olympiads"]) {
        if (olympiad["code"] != olympiad_code) {
          continue;
        }

        for (auto& task : olympiad["tasks"]) {
          if (task["code"] == code) {
            std::cerr << argv[0] << ": task '" << code << "' already exists"
                      << std::endl;
            return 1;
          }
        }

        ops::task::Task task(code, name);

        auto result = task.create_task_folder();

        if (!result.success()) {
          std::cerr << argv[0] << ": failed to create task '" << code
                    << "' due to " << result.output << std::endl;
          return 1;
        }

        olympiad["tasks"].push_back({{"code", code}, {"name", name}});
        ops["current_task"] = code;

        save_state(ops);
        std::cout << argv[0] << ": task '" << code << "' created successfully"
                  << std::endl;
        return 0;
      }

      std::cerr << argv[0] << ": olympiad '" << olympiad_code
                << "' does not exist" << std::endl;
      return 1;
    } else if (action == "list") {
      auto olympiad_code = ops["current_olympiad"].get<std::string>();

      for (auto& olympiad : ops["olympiads"]) {
        if (olympiad["code"] != olympiad_code) {
          continue;
        }

        for (auto& task : olympiad["tasks"]) {
          bool current = task["code"] == ops["current_task"];

          std::cout << (current ? "* " : "  ")
                    << task["code"].get<std::string>() << std::endl;
        }

        return 0;
      }

      std::cerr << argv[0] << ": olympiad '" << olympiad_code
                << "' does not exist" << std::endl;
      return 1;
    } else if (action == "use") {
      if (argc < 4) {
        std::cerr << "usage: " << argv[0] << " task use <code>" << std::endl;
        return 1;
      }

      std::string code = argv[3];

      auto olympiad_code = ops["current_olympiad"].get<std::string>();

      for (auto& olympiad : ops["olympiads"]) {
        if (olympiad["code"] != olympiad_code) {
          continue;
        }

        for (auto& task : olympiad["tasks"]) {
          if (task["code"] == code) {
            ops["current_task"] = code;
            save_state(ops);

            std::cout << argv[0] << ": using task '" << code << "'"
                      << std::endl;
            return 0;
          }
        }

        std::cerr << argv[0] << ": task '" << code
                  << "' does not exist in olympiad '" << olympiad_code << "'"
                  << std::endl;
        return 1;
      }

      std::cerr << argv[0] << ": olympiad '" << olympiad_code
                << "' does not exist" << std::endl;
      return 1;
    } else {
      std::cerr << argv[0] << ": task '" << action
                << "' is not a valid action. See '" << argv[0] << " help'."
                << std::endl;
      return 1;
    }
  }

  std::cerr << argv[0] << ": '" << entity << "' is not a valid command. See '"
            << argv[0] << " help'." << std::endl;
  return 1;
}