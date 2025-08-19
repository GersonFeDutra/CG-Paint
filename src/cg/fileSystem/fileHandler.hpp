#pragma once
#include <tuple>

#include "cg/canvas_itens/line.hpp"
#include "cg/canvas_itens/point.hpp"
#include "cg/canvas_itens/polygon.hpp"

namespace cg
{
    class FileHandler {
    public:
        FileHandler() {};

        void saveFile(ArrayList<std::unique_ptr<cg::Point>> pointList, ArrayList<std::unique_ptr<cg::Line>> linesList, ArrayList<std::unique_ptr<cg::Polygon>> polygonList);
        std::tuple<ArrayList<std::unique_ptr<cg::Point>>, ArrayList<std::unique_ptr<cg::Line>>, ArrayList<std::unique_ptr<cg::Polygon>>> loadFile(std::string path_to_file);
    private:
        std::string defaultFileLocation = "saved_shapes.objx";
    };
} // namespace files
