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
        ArrayList<cg::Point *> pointsArray;
        ArrayList<cg::Line *> linesArray;
        ArrayList<cg::Polygon *> polygonsArray;

        if (path_to_file.empty()) {
            path_to_file = defaultFileLocation;
        }

        std::ifstream inputFile(path_to_file);

        if (inputFile.is_open()) {
            std::string currentLine, info;
            std::vector<std::string> lineInfo;
            char token = ':';
            
            while (std::getline(inputFile, currentLine)) {
                if (currentLine.empty()) continue;
                
                lineInfo.clear();
                std::vector<std::string> thingsInfo;
                std::string tempString;
                
                std::istringstream tokenLine(currentLine);
                
                while (std::getline(tokenLine, info, token)) {
                    lineInfo.push_back(info);
                }
                
                if (lineInfo[0] == "PNT") {
                    std::istringstream line01(lineInfo[1]);
                    std::string positionInfo;
                    while (std::getline(line01, positionInfo, ',')) {
                        thingsInfo.push_back(positionInfo);
                    }
                    Vector2 pos(std::stof(thingsInfo[0]), std::stof(thingsInfo[1]));
                    thingsInfo.clear();
                    
                    Point newPoint;
                    newPoint.setPosition(pos);

                    // newPoint->SIZE = lineInfo[2]; // por enquanto o tamanho não muda, caso mude é só alterar aqui
                    
                    // std::cout << lineInfo[3] << std::endl;
                    std::istringstream line03(lineInfo[3]);
                    while (std::getline(line03, info, ',')) {
                        if (info.empty()) continue;
                        thingsInfo.push_back(info);
                    }
                    for (std::string pop : thingsInfo) {
                        std::cout << pop << std::endl;
                    }
                    ColorRgb pointColor((unsigned char) std::stoi(thingsInfo[0]), (unsigned char) std::stoi(thingsInfo[1]), (unsigned char) std::stoi(thingsInfo[2]));
                    newPoint.setColor(pointColor);

                    pointsArray.push_back(&newPoint);
                } else if (lineInfo[0] == "LIN") {
                    Line* newLine;

                } else if (lineInfo[0] == "POL") {
                    Polygon* newPoly;

                }
            }

            inputFile.close();

            return std::make_tuple(pointsArray, linesArray, polygonsArray);
        } else {
            std::cout << "ERROR! COULDN'T READ FILE" << std::endl;

            // return;
        }

    }

    void FileHandler::saveFile(ArrayList<cg::Point *> pointList, ArrayList<cg::Line *> linesList, ArrayList<cg::Polygon *> polygonList) {
        std::ofstream outputFile(defaultFileLocation);

        if (outputFile.is_open()) {
            for (Point* obj : pointList) {
                outputFile << "PNT:" << obj->position.x << "," << obj->position.y << ":" << obj->SIZE << ":" << (int) obj->getColor().r << "," << (int) obj->getColor().g << "," << (int) obj->getColor().b << std::endl;
            }
            
            for (Line* obj : linesList) {
                std::string vertices;
                for (auto& vertex : obj->getVertices()) {
                    vertices.append(std::to_string(vertex.x));
                    vertices.append(",");
                    vertices.append(std::to_string(vertex.y));
                    vertices.append(";");
                }
                outputFile << "LIN:" << obj->position.x << "," << obj->position.y << ":" << vertices << ":" << (int) obj->getColor().r << "," << (int) obj->getColor().g << "," << (int) obj->getColor().b << std::endl;
            }
            
            for (Polygon* obj : polygonList) {
                std::string vertices;
                for (auto& vertex : obj->getVertices()) {
                    vertices.append(std::to_string(vertex.x));
                    vertices.append(",");
                    vertices.append(std::to_string(vertex.y));
                    vertices.append(";");
                }

                outputFile << "POL:" << obj->position.x << "," << obj->position.y << ":" << vertices << ":" << (int) obj->getColor().r << "," << (int) obj->getColor().g << "," << (int) obj->getColor().b << std::endl;
            }           
            
            outputFile.close();
        } else {
            std::cout << "FILE WAS NOT WRITTEN/CREATED" << std::endl;
        }
    }
} // namespace files
