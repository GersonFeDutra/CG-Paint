#pragma once
#include <tuple>

#include <cg/canvas_itens/line.hpp>
#include <cg/canvas_itens/point.hpp>
#include <cg/canvas_itens/polygon.hpp>

namespace cg
{
    class FileHandler {
    public:
        FileHandler() {};

        void saveFile(ArrayList<Point *> pointList, ArrayList<Line *> linesList, ArrayList<Polygon *> polygonList);
        std::tuple<ArrayList<Point *>, ArrayList<Line *>, ArrayList<Polygon *>> loadFile(std::string path_to_file);
    private:
        // TODO -> Save in default system user:// path
        std::string defaultFileLocation = "saved_shapes.objx";
    };
} // namespace files
