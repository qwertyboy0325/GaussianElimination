#include "FilepathTool.h"

bool isValidPath(const std::string& path) {
    std::filesystem::path filePath(path);

    if (std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath)) {
        std::string extension = filePath.extension().string();
        if (extension == ".txt") {
            return true;
        }
    }

    return false;
}