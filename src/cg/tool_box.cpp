#include "tool_box.hpp"

#include "canvas.hpp"

#include "canvas_itens/point.hpp"
#include "canvas_itens/line.hpp"
#include "canvas_itens/polygon.hpp"

#include "tools/point_tool.hpp"
#include "tools/line_tool.hpp"
#include "tools/polygon_tool.hpp"

#include <facade/gui.hpp>


namespace cg {

    ToolBox::ToolBox() : tools{nullptr, nullptr, nullptr} {}

    void ToolBox::addCanvas(Canvas* canvas_ptr) {
        assert_err(canvas == nullptr, "Canvas already added.");
        canvas = canvas_ptr;

        // Declare as ferramentas aqui.
        static PointTool pointTool{ *this };
        static LineTool lineTool{ *this };
        static PolygonTool polygonTool{ *this };

        // Passe as ferramentas para o array (lembre-se de alterar o tamanho dele em N_PRIMITIVES).
        tools = { (Painter*)&pointTool, (Painter*)&lineTool, (Painter*)&polygonTool };
    }

    void ToolBox::_render()
    {
        enum { NONE, SAVE, LOAD } clicked = NONE;
        // Cada janela deve ser criada num escopo separado para
        // invocar os construtores/ destrutores necessários
        {  // Mostra uma janela simples.
            static int counter = 0;
            static float f = 0.0f;

            Window toolBox("Controls");
            // TODO -> Use Icon buttons for each tool

            // toolBox.showSliderFloat(&f, "float");
            // toolBox.showCheckBox(&check, "Active

            // glut cursor
            static unsigned tool_cursor = GLUT_CURSOR_INHERIT;
            static unsigned last_cursor = GLUT_CURSOR_INHERIT;
            unsigned current_cursor = GLUT_CURSOR_INHERIT;

            if (toolBox.showRadioButton(&currentPrimitive, cg::ToolBox::Primitives::POINT, "Point")) {
                tool_cursor = current_cursor;
            } // use point
            if (toolBox.showRadioButton(&currentPrimitive, cg::ToolBox::Primitives::LINE, "Line")) {
                tool_cursor = GLUT_CURSOR_CROSSHAIR;
            } // use line
            if (toolBox.showRadioButton(&currentPrimitive, cg::ToolBox::Primitives::POLYGON, "Polygon")) {
                tool_cursor = GLUT_CURSOR_CROSSHAIR;
            } // use polygon

            if (!isInsideGui)
                current_cursor = tool_cursor;

            if (current_cursor != last_cursor) {
                glutSetCursor(current_cursor);
                last_cursor = current_cursor;
            }

            // TODO [Extra] -> Polígono regular
            // switch (canvas.toolBox.currentPrimitive) {
            // case cg::ToolBox::Primitives::REGULAR_POLYGON: {
            //     toolBox.sameLine();
            //     toolBox.showSliderInt(&canvas.toolBox.polygonEdges, 1, 255, "Edges");
            //     toolBox.sameLine();
            //     toolBox.showText("[%d]", canvas.toolBox.polygonEdges);
            // } break;
            // default: break;
            // }

            toolBox.showColorEdit((cg::Vector3*)&(getColorPtr()->r), "color");

            //if (toolBox.showButton("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
            //    counter++;
            //toolBox.sameLine();
            //toolBox.showText("counter = %d", counter);

            if (toolBox.showButton("Save to file"))
                clicked = SAVE;

            if (toolBox.showButton("Load from file"))
                clicked = LOAD;

            toolBox.showText("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / Gui::getFps(), Gui::getFps());
        }

        switch (clicked) {
        case SAVE:
            save();
            break;
        case LOAD:
            load();
            break;
        default:
            break;
        }

        if (currentPrimitive >= N_PRIMITIVES)
            return;
        tools[currentPrimitive]->_render();
    }

    void ToolBox::captureInput(io::MouseMove input_event)
    {
        if (currentPrimitive >= N_PRIMITIVES)
            return;
        if (currentTool != currentPrimitive) {
            // TODO -> update using tool
            currentTool = currentPrimitive;
        }

        tools[currentPrimitive]->_input(input_event);
    }

    void ToolBox::captureInput(io::MouseDrag input_event)
    {
        if (currentPrimitive >= N_PRIMITIVES)
            return;
        if (currentTool != currentPrimitive) {
            // TODO -> update using tool
            currentTool = currentPrimitive;
        }

        tools[currentPrimitive]->_input(input_event);
    }

    void ToolBox::captureInput(io::MouseLeftButtonPressed input_event)
    {
        if (currentPrimitive >= N_PRIMITIVES)
            return;
        if (currentTool != currentPrimitive) {
            // TODO -> update using tool
            currentTool = currentPrimitive;
        }
        // std::cout << "Using left button" << std::endl;

        tools[currentPrimitive]->_input(input_event);
    }

    void ToolBox::captureInput(io::MouseLeftButtonReleased input_event)
    {
        if (currentPrimitive >= N_PRIMITIVES)
            return;
        if (currentTool != currentPrimitive) {
            // TODO -> update using tool
            currentTool = currentPrimitive;
        }

        tools[currentPrimitive]->_input(input_event);
    }

    void ToolBox::captureInput(io::MouseRightButtonPressed input_event)
    {
        // std::cout << N_PRIMITIVES << ", " << currentTool << ", " << currentPrimitive << std::endl;
        if (currentPrimitive >= N_PRIMITIVES)
            return;
        if (currentTool != currentPrimitive) {
            // TODO -> update using tool
            currentTool = currentPrimitive;
        }

        if (currentTool != POINT) {
            // std::cout << "Using right button" << std::endl;
            tools[currentPrimitive]->_input(input_event);
        }
    }

    void ToolBox::save()
    {
        Gui::saveFileDialog("SaveFile", "Salvando arquivo...", ".tcgp,.cgp", [&](std::ofstream& ofs) {
            ArrayList<Point *> points;
            ArrayList<Line *> lines;
            ArrayList<Polygon *> polygons;

            points.reserve(canvas->getTypeCount(CanvasItem::TypeInfo::POINT));
            lines.reserve(canvas->getTypeCount(CanvasItem::TypeInfo::LINE));
            polygons.reserve(canvas->getTypeCount(CanvasItem::TypeInfo::POLYGON));

            for (auto& item : canvas->getItens()) {
                switch (item->getTypeInfo()) {

                case CanvasItem::TypeInfo::POINT: {
                    auto* derivedClass = dynamic_cast<Point *>(item.get());
                    assert_err(derivedClass != nullptr, "Point convertion failed!");
                    points.emplace_back(derivedClass);
                } break;

                case CanvasItem::TypeInfo::LINE: {
                    auto* derivedClass = dynamic_cast<Line *>(item.get());
                    assert_err(derivedClass != nullptr, "Line convertion failed!");
                    lines.emplace_back(derivedClass);
                } break;

                case CanvasItem::TypeInfo::POLYGON: {
                    auto* derivedClass = dynamic_cast<Polygon *>(item.get());
                    assert_err(derivedClass != nullptr, "Polygon convertion failed!");
                    polygons.emplace_back(derivedClass);
                } break;

                default: {
                    continue; // ignore
                } break;
                }
            }

			ofs << "Points: [";
            if (points.empty())
                ofs << ' ';
            else
                for (auto& point : points)
                    ofs << *point << '\n';
            ofs << "]\n";

            ofs << "Lines: [";
            if (lines.empty())
                ofs << ' ';
			else
                for (auto& line : lines)
                    ofs << *line << '\n';
            ofs << "]\n";

            ofs << "Polygons: [";
			if (polygons.empty())
				ofs << ' ';
            else
                for (auto& polygon : polygons)
				    ofs << *polygon << '\n';
			ofs << "]\n";
        });
    }

    void ToolBox::load()
    {
        Gui::openFileDialog("OpenFile", "Escolha um arquivo...", ".tcgp,.cgp", [&](std::ifstream& ifs) {
            if (ifs.eof()) {
				print_warning("File is empty or not found.");
                return;
            }

			canvas->clear(); // Clear the canvas before loading new items

            std::string dummie;
			ifs >> dummie; // Read "Points:"
            if (dummie != "Points:")
                print_warning("Invalid file format. Expected 'Points:' but got '%s', continuing...", dummie.c_str());
            else {
				ifs >> dummie; // Read "["
                if (dummie != "[")
                    print_warning("Invalid file format. Expected '[' but got '%s', continuing...", dummie.c_str());
                else {
                    while (ifs.peek() != ']') {
                        if (ifs.eof()) {
                            print_warning("Unexpected end of file while reading points.");
                            return;
						}
                        auto point = std::make_unique<cg::Point>();
                        point->_deserialize(ifs);
						canvas->insert(std::move(point));
                    }
                }
            }
			ifs >> dummie; // Read "Lines:"
            if (dummie != "Lines:")
                print_warning("Invalid file format. Expected 'Lines:' but got '%s', continuing...", dummie.c_str());
            else {
                ifs >> dummie; // Read "["
                if (dummie != "[")
                    print_warning("Invalid file format. Expected '[' but got '%s', continuing...", dummie.c_str());
                else {
                    while (ifs.peek() != ']') {
                        if (ifs.eof()) {
                            print_warning("Unexpected end of file while reading lines.");
                            return;
                        }
                        auto line = std::make_unique<cg::Line>();
                        line->_deserialize(ifs);
                        canvas->insert(std::move(line));
                    }
                }
			}
			ifs >> dummie; // Read "Polygons:"
            if (dummie != "Polygons:")
                print_warning("Invalid file format. Expected 'Polygons:' but got '%s', continuing...", dummie.c_str());
            else {
				ifs >> dummie; // Read "["
                if (dummie != "[")
                    print_warning("Invalid file format. Expected '[' but got '%s', continuing...", dummie.c_str());
                else {
                    while (ifs.peek() != ']') {
                        if (ifs.eof()) {
                            print_warning("Unexpected end of file while reading polygons.");
                            return;
                        }
                        auto polygon = std::make_unique<cg::Polygon>();
                        polygon->_deserialize(ifs);
                        canvas->insert(std::move(polygon));
                    }
				}
			}

        });
    }

} // namespace cg