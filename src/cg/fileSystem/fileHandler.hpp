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

        void saveFile(ArrayList<cg::Point *> pointList, ArrayList<cg::Line *> linesList, ArrayList<cg::Polygon *> polygonList);
        std::tuple<ArrayList<cg::Point *>, ArrayList<cg::Line *>, ArrayList<cg::Polygon *>> loadFile(std::string path_to_file);
    private:
        std::string defaultFileLocation = "saved_shapes.objx";
    };
} // namespace files
