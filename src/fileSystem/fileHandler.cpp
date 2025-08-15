#include <fstream>
#include <iostream>

#include "fileHandler.hpp"
#include "cg/canvas.hpp"

#include "cg/canvas_itens/line.hpp"
#include "cg/canvas_itens/point.hpp"
#include "cg/canvas_itens/polygon.hpp"

namespace files
{
    void FileHandler::loadFile(std::string path_to_file, cg::Canvas currentCanvas) {

    }

    void FileHandler::saveFile(cg::Canvas currentCanvas) {
        ArrayList<cg::Polygon *> polygons = currentCanvas.getItemsOfType<cg::Polygon>();
        ArrayList<cg::Line *> lines = currentCanvas.getItemsOfType<cg::Line>();
        ArrayList<cg::Point *> points = currentCanvas.getItemsOfType<cg::Point>();

        std::cout << "Polygons:" << std::endl;
        for (auto& plgn : polygons) {
            std::cout << plgn->position.x << ", " << plgn->position.y << std::endl;
        }
        
        std::cout << "\nLines: " << std::endl;
        for (auto& lns : lines) {
            std::cout << lns->position.x << ", " << lns->position.y << std::endl;
        }

        std::cout << "\nPoints: " << std::endl;
        for (auto& pnts : points) {
            std::cout << pnts->position.x << ", " << pnts->position.y << std::endl;
        }
        // const auto& canvasItems = currentCanvas.getItens(); 
        // for (auto& object : canvasItems) {
        //     if (auto a = dynamic_cast<cg::Polygon*>(object.get())) {
        //         std::cout << "Polygon" << std::endl;
        //     } else if (auto b = dynamic_cast<cg::Point*>(object.get())) {
        //         std::cout << "Point" << std::endl;
        //     } else if (auto c = dynamic_cast<cg::Line*>(object.get())) {
        //         std::cout << "Line" << std::endl;
        //     }
        // }
    }
} // namespace files
