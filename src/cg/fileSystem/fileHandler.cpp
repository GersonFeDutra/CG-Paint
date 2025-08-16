#include <iostream>
#include <fstream>
#include <filesystem>

#include "fileHandler.hpp"

#include "cg/canvas_itens/line.hpp"
#include "cg/canvas_itens/point.hpp"
#include "cg/canvas_itens/polygon.hpp"

namespace cg
{
    std::tuple<ArrayList<cg::Point *>, ArrayList<cg::Line *>, ArrayList<cg::Polygon *>> FileHandler::loadFile(std::string path_to_file) {

    }

    void FileHandler::saveFile(ArrayList<cg::Point *> pointList, ArrayList<cg::Line *> linesList, ArrayList<cg::Polygon *> polygonList) {
        std::ofstream outputFile(defaultFileLocation);

        if (outputFile.is_open()) {
            outputFile << "START_POINTS" << std::endl;
            for (Point* obj : pointList) {
                outputFile << "(" << obj->position.x << ", " << obj->position.y << "):" << obj->SIZE << ":" << (int) obj->getColor().r << "," << (int) obj->getColor().g << "," << (int) obj->getColor().b << "" << std::endl;
            }
            outputFile << "END_POINTS" << std::endl;
            
            outputFile << "START_LINES" << std::endl;
            for (Line* obj : linesList) {
                std::string vertices;
                for (auto& vertex : obj->getVertices()) {
                    vertices.append("(");
                    vertices.append(std::to_string(vertex.x));
                    vertices.append(", ");
                    vertices.append(std::to_string(vertex.y));
                    vertices.append(");");
                }
                outputFile << "(" << obj->position.x << ", " << obj->position.y << "):" << vertices << ":" << (int) obj->getColor().r << "," << (int) obj->getColor().g << "," << (int) obj->getColor().b << "" << std::endl;
            }
            outputFile << "END_LINES" << std::endl;
            
            outputFile << "START_POLYGONS" << std::endl;
            for (Polygon* obj : polygonList) {
                std::string vertices;
                for (auto& vertex : obj->getVertices()) {
                    vertices.append("(");
                    vertices.append(std::to_string(vertex.x));
                    vertices.append(", ");
                    vertices.append(std::to_string(vertex.y));
                    vertices.append(");");
                }

                outputFile << "(" << obj->position.x << ", " << obj->position.y << "):" << vertices << ":" << (int) obj->getColor().r << "," << (int) obj->getColor().g << "," << (int) obj->getColor().b << "" << std::endl;
            }
            outputFile << "END_POLYGONS" << std::endl;            
            
            outputFile.close();
        } else {
            std::cout << "FILE WAS NOT WRITTEN/CREATED" << std::endl;
        }
    }
} // namespace files
