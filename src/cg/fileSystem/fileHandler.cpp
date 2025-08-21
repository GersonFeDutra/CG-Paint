#include <iostream>
#include <fstream>
#include <filesystem>

#include <memory>

#include <cmath>

#include "fileHandler.hpp"

#include <cg/math.hpp>
#include <cg/canvas_itens/line.hpp>
#include <cg/canvas_itens/point.hpp>
#include <cg/canvas_itens/polygon.hpp>

namespace cg
{
    std::tuple<ArrayList<Point *>, ArrayList<Line *>, ArrayList<Polygon *>> FileHandler::loadFile(std::string path_to_file) {
        ArrayList<Point *> pointsArray;
        ArrayList<Line *> linesArray;
        ArrayList<Polygon *> polygonsArray;

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
                    
                    std::istringstream line03(lineInfo[3]);
                    while (std::getline(line03, info, ',')) {
                        if (info.empty()) continue;
                        thingsInfo.push_back(info);
                    }
                    
                    ColorRgb pointColor((unsigned char) std::stoi(thingsInfo[0]), (unsigned char) std::stoi(thingsInfo[1]), (unsigned char) std::stoi(thingsInfo[2]));
                    
                    Point * newPoint = new Point();
                    
                    newPoint->setColor(pointColor);
                    newPoint->setPosition(pos);

                    pointsArray.push_back(newPoint);
                } else if (lineInfo[0] == "LIN") {
                    std::istringstream line01(lineInfo[1]);
                    std::string positionInfo;

                    while (std::getline(line01, positionInfo, ',')) {
                        thingsInfo.push_back(positionInfo);
                    }
                    Vector2 pos(std::stof(thingsInfo[0]), std::stof(thingsInfo[1]));
                    thingsInfo.clear();

                    std::vector<Vector2> lineVertex;
                    std::istringstream line02(lineInfo[2]);
                    while (std::getline(line02, info, ';')) {
                        std::istringstream infoVertex(info);
                        std::string tempstr;

                        while (std::getline(infoVertex, tempstr, ',')) {
                            thingsInfo.push_back(tempstr);
                        }

                        Vector2 vector(std::stof(thingsInfo[0]), std::stof(thingsInfo[1]));
                        lineVertex.push_back(vector);
                        thingsInfo.clear();
                    }
                    
                    std::istringstream line03(lineInfo[3]);
                    while (std::getline(line03, info, ',')) {
                        if (info.empty()) continue;
                        thingsInfo.push_back(info);
                    }
                    
                    ColorRgb lineColor((unsigned char) std::stoi(thingsInfo[0]), (unsigned char) std::stoi(thingsInfo[1]), (unsigned char) std::stoi(thingsInfo[2]));
                    
                    Line* newLine = new Line();
                    
                    newLine->setColor(lineColor);
                    newLine->setPosition(pos);
                    newLine->setVertices(lineVertex);

                    linesArray.push_back(newLine);
                } else if (lineInfo[0] == "POL") {
                    std::istringstream line01(lineInfo[1]);
                    std::string positionInfo;

                    while (std::getline(line01, positionInfo, ',')) {
                        thingsInfo.push_back(positionInfo);
                    }
                    Vector2 pos(std::stof(thingsInfo[0]), std::stof(thingsInfo[1]));
                    thingsInfo.clear();

                    std::vector<Vector2> polyVertex;
                    std::istringstream line02(lineInfo[2]);
                    while (std::getline(line02, info, ';')) {
                        std::istringstream infoVertex(info);
                        std::string tempstr;

                        while (std::getline(infoVertex, tempstr, ',')) {
                            thingsInfo.push_back(tempstr);
                        }

                        Vector2 vector(std::stof(thingsInfo[0]), std::stof(thingsInfo[1]));
                        polyVertex.push_back(vector);
                        thingsInfo.clear();
                    }
                    
                    std::istringstream line03(lineInfo[3]);
                    while (std::getline(line03, info, ',')) {
                        if (info.empty()) continue;
                        thingsInfo.push_back(info);
                    }
                    
                    ColorRgb polyColor((unsigned char) std::stoi(thingsInfo[0]), (unsigned char) std::stoi(thingsInfo[1]), (unsigned char) std::stoi(thingsInfo[2]));
                    
                    Polygon* newPoly = new Polygon();
                    
                    newPoly->setColor(polyColor);
                    newPoly->setPosition(pos);
                    newPoly->setVertices(polyVertex);

                    polygonsArray.push_back(newPoly);
                }
            }

            inputFile.close();

            return std::make_tuple(pointsArray, linesArray, polygonsArray);
        } else {
            std::cout << "ERROR! COULDN'T READ FILE" << std::endl;

            return std::make_tuple(pointsArray, linesArray, polygonsArray);
        }

    }

    void FileHandler::saveFile(ArrayList<cg::Point *> pointList, ArrayList<cg::Line *> linesList, ArrayList<cg::Polygon *> polygonList) {
        std::ofstream outputFile(defaultFileLocation);

        if (outputFile.is_open()) {
            for (const auto& obj : pointList) {
                outputFile << "PNT:" << std::round(obj->position.x * 100) / 100 << "," << std::round(obj->position.y * 100) / 100 << ":" << obj->SIZE << ":" << (int) obj->getColor().r << "," << (int) obj->getColor().g << "," << (int) obj->getColor().b << std::endl;
            }
            
            for (const auto& obj : linesList) {
                std::string vertices;
                for (auto& vertex : obj->getVertices()) {
                    vertices.append(std::to_string(vertex.x));
                    vertices.append(",");
                    vertices.append(std::to_string(vertex.y));
                    vertices.append(";");
                }
                outputFile << "LIN:" << std::round(obj->position.x * 100) / 100 << "," << std::round(obj->position.y * 100) / 100 << ":" << vertices << ":" << (int) obj->getColor().r << "," << (int) obj->getColor().g << "," << (int) obj->getColor().b << std::endl;
            }
            
            for (const auto& obj : polygonList) {
                std::string vertices;
                for (auto& vertex : obj->getVertices()) {
                    vertices.append(std::to_string(vertex.x));
                    vertices.append(",");
                    vertices.append(std::to_string(vertex.y));
                    vertices.append(";");
                }

                outputFile << "POL:" << std::round(obj->position.x * 100) / 100 << "," << std::round(obj->position.y * 100) / 100 << ":" << vertices << ":" << (int) obj->getColor().r << "," << (int) obj->getColor().g << "," << (int) obj->getColor().b << std::endl;
            }
            
            outputFile.close();
        } else {
            std::cout << "FILE WAS NOT WRITTEN/CREATED" << std::endl;
        }
    }
} // namespace files
