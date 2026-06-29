#include <iostream>

#include "all"

int main(int argc, char* argv[]) { 
    
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <command> [args...]" << std::endl;
        return 1;
    }

    std::string command = argv[1];
    
    if (command == "task") {
        if (argc < 4) {
            std::cerr << "Usage: " << argv[0] << " task <code> <name>" << std::endl;
            return 1;
        }
        std::string code = argv[2];
        std::string name = argv[3];
        ops::task::Task task(code, name);
        if (!task.create_task_folder()) {
            return 1;
        }
    } 

    return 0; 
}