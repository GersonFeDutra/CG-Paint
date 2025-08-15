#pragma once

namespace cg
{
    class FileHandler {
    public:
        FileHandler() {};

        void saveFile();
        void loadFile(std::string path_to_file);
    private:
        std::string defaultFileLocation = "./savedFiles/";
    };
} // namespace files
