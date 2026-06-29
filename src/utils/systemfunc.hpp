#ifndef OPS_UTILS_HPP
#define OPS_UTILS_HPP 1

#include <array>
#include <memory>
#include <string>

namespace ops::utils {

struct ExecResult {
  int exit_code;
  std::string output;

  bool success() const {
    return exit_code == 0;
  }
};

ExecResult exec_bash_function(const std::string& command) {
    ExecResult result;
    std::array<char, 256> buffer{};

    std::string cmd = command + " 2>&1";

    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        result.output = "Failed to start command";
        return result;
    }

    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
        result.output += buffer.data();
    }

    int status = pclose(pipe);
    if (status == -1) {
        result.output += "Failed to close command pipe";
        return result;
    }

    if (WIFEXITED(status)) {
        result.exit_code = WEXITSTATUS(status);
    } else {
        result.exit_code = -1;
    }

    return result;
}

ExecResult create_directory(const std::string& path) {
  return exec_bash_function("mkdir " + path);
}

ExecResult file_exists(const std::string& path) {
  return exec_bash_function("[ -f " + path + " ]");
}

}  // namespace ops::utils

#endif  // OPS_UTILS_HPP