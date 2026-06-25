#ifndef OPS_UTILS_HPP
#define OPS_UTILS_HPP 1

#include <array>
#include <memory>
#include <string>

namespace ops::utils {

std::string exec_bash_function(const std::string& cmd) {
  std::array<char, 128> buffer;
  std::string result;

  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"),
                                                pclose);
  if (!pipe) {
    throw std::runtime_error("popen() failed!");
  }

  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  
  return result;
}

bool create_directory(const std::string& path) {
  std::string cmd = "mkdir -p " + path;
  std::string ret = exec_bash_function(cmd);
  return ret.empty();
}

}  // namespace ops::utils

#endif  // OPS_UTILS_HPP