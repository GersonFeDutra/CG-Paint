#pragma once

#include "cg/canvas.hpp"

namespace files
{
    class FileHandler {
    public:
        FileHandler() {};

        void saveFile(cg::Canvas currentCanvas);
        void loadFile(std::string path_to_file, cg::Canvas currentCanvas);
    private:
        std::string defaultFileLocation = "./savedFiles/";
    };
} // namespace files
