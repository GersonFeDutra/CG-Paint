#include "tool_box.hpp"

#include "canvas.hpp"

#include "canvas_itens/point.hpp"
#include "canvas_itens/line.hpp"
#include "canvas_itens/polygon.hpp"

#include "tools/point_tool.hpp"
#include "tools/line_tool.hpp"
#include "tools/polygon_tool.hpp"
#include "tools/select_tool.hpp" 

#include <facade/gui.hpp>


namespace cg {

    ToolBox::ToolBox() : tools{nullptr, nullptr, nullptr, nullptr} {}

    void ToolBox::addCanvas(Canvas* canvas_ptr) {
        assert_err(canvas == nullptr, "Canvas already added.");
        canvas = canvas_ptr;

        // Declare as ferramentas aqui.
        static PointTool pointTool{ *this };
        static LineTool lineTool{ *this };
        static PolygonTool polygonTool{ *this };
        static SelectTool selectTool{ *this };
        
        // Passe as ferramentas para o array (lembre-se de alterar o tamanho dele em N_PRIMITIVES).
        tools = { (Painter*)&pointTool, (Painter*)&lineTool, (Painter*)&polygonTool, (Painter*)&selectTool };
        
    }

    static unsigned tool_cursor = GLUT_CURSOR_INHERIT;
    static unsigned last_cursor = GLUT_CURSOR_INHERIT;

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
            unsigned current_cursor = GLUT_CURSOR_INHERIT;

            int _currentTool = currentTool;
            if (toolBox.showRadioButton(&_currentTool, Tools::POINT, "Point [F1]")) {
				std::cout << "Point tool selected\n";
                tool_cursor = GLUT_CURSOR_NONE;
            } // use point
            if (toolBox.showRadioButton(&_currentTool, Tools::LINE, "Line [F2]")) {
                tool_cursor = GLUT_CURSOR_CROSSHAIR;
            } // use line
            if (toolBox.showRadioButton(&_currentTool, Tools::POLYGON, "Polygon [F3]")) {
                tool_cursor = GLUT_CURSOR_CROSSHAIR;
            } // use polygon
            if (toolBox.showRadioButton(&_currentTool, Tools::SELECT, "Select [F4]")) {
                tool_cursor = GLUT_CURSOR_INHERIT;
			} // selection tool
            if (_currentTool != currentTool) {
                currentTool = _currentTool;
				// Do something on tool change
			}

            if (!isInsideGui)
                current_cursor = tool_cursor;

            if (current_cursor != last_cursor) {
                glutSetCursor(current_cursor);
                last_cursor = current_cursor;
            }

            // TODO [Extra] -> Polígono regular
            // switch (canvas.toolBox._currentTool) {
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
        case SAVE: {
            save();
        } break;
        case LOAD: {
            load();
        } break;
        default:
            break;
        }
        if (currentTool >= N_PRIMITIVES)
            return;
        tools[currentTool]->_render();
    }

    void ToolBox::captureInput(io::MouseMove input_event)
    {
        if (currentTool >= N_PRIMITIVES)
            return;

        tools[currentTool]->_input(input_event);
    }

    void ToolBox::captureInput(io::MouseDrag input_event)
    {
        if (currentTool >= N_PRIMITIVES)
            return;

        tools[currentTool]->_input(input_event);
    }

    void ToolBox::captureInput(io::MouseLeftButtonPressed input_event)
    {
        if (currentTool >= N_PRIMITIVES)
            return;

        tools[currentTool]->_input(input_event);
    }

    void ToolBox::captureInput(io::MouseLeftButtonReleased input_event)
    {
        if (currentTool >= N_PRIMITIVES)
            return;

        tools[currentTool]->_input(input_event);
    }

    void ToolBox::captureInput(io::MouseRightButtonPressed input_event)
    {
        if (currentTool >= N_PRIMITIVES)
            return;

        if (currentTool != POINT)
            tools[currentTool]->_input(input_event);
    }

    void ToolBox::captureInput(io::SpecialKeyInputEvent input_event)
    {
        switch (input_event.key) {
        case GLUT_KEY_F1: {
            tool_cursor = GLUT_CURSOR_NONE;
            currentTool = Tools::POINT;
        } break;
        case GLUT_KEY_F2: {
            tool_cursor = GLUT_CURSOR_CROSSHAIR;
            currentTool = Tools::LINE;
        } break;
        case GLUT_KEY_F3: {
            tool_cursor = GLUT_CURSOR_CROSSHAIR;
            currentTool = Tools::POLYGON;
        } break;
        case GLUT_KEY_F4: {
			tool_cursor = GLUT_CURSOR_INHERIT;
			currentTool = Tools::SELECT;
        } break;
        default: // ignore
            break;
        }
    }

    void ToolBox::captureInput(io::KeyboardInputEvent input_event)
    {
        const unsigned char ESC = 27;

        if (input_event.mods & GLUT_ACTIVE_CTRL) {
            switch (input_event.key) {
            case 's':
                save();
                break;
            case 'o':
                load();
                break;
            default: // ignore
                break;
			}
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
                while (ifs.peek() == ' ' || ifs.peek() == '\n' || ifs.peek() == '\r')
                    ifs.ignore(); // Ignore leading whitespace

                if (char s = ifs.peek(); s != '[')
                    print_warning("Invalid file format. Expected '[' but got '%c', continuing...", s);
                else {
                    ifs.ignore(); // Ignore '['
                    while (ifs.peek() == ' ' || ifs.peek() == '\n' || ifs.peek() == '\r')
                        ifs.ignore(); // Ignore leading whitespace

                    if (char s = ifs.peek(); s != ']') {
                        while (ifs.peek() != ']') {
                            auto point = std::make_unique<cg::Point>();
                            try {
                                point->_deserialize(ifs);
                            }
                            catch (const std::exception& e) {
                                print_error("Failed to deserialize point: %s", e.what());
                                return;
							}
                            if (ifs.eof()) {
                                print_warning("Unexpected end of file while reading points.");
                                return;
						    }
						    canvas->insert(std::move(point));
                            while (ifs.peek() == ' ' || ifs.peek() == '\n' || ifs.peek() == '\r')
                                ifs.ignore(); // Ignore leading whitespace
                        }
                        if (ifs.peek() == ']') {
                            ifs.ignore(); // Ignore ']'
                        }
						else
							print_warning("Invalid file format. Expected ']' but got '%c', continuing...", ifs.peek());
                    }
                    else
						ifs.ignore(); // empty
                }
            }
			ifs >> dummie; // Read "Lines:"
            if (dummie != "Lines:")
                print_warning("Invalid file format. Expected 'Lines:' but got '%s', continuing...", dummie.c_str());
            else {
                while (ifs.peek() == ' ' || ifs.peek() == '\n' || ifs.peek() == '\r')
                    ifs.ignore(); // Ignore leading whitespace

                if (char s = ifs.peek(); s != '[')
                    print_warning("Invalid file format. Expected '[' but got '%c', continuing...", s);
                else {
                    ifs.ignore(); // Ignore '['
                    while (ifs.peek() == ' ' || ifs.peek() == '\n' || ifs.peek() == '\r')
                        ifs.ignore(); // Ignore leading whitespace

                    if (char s = ifs.peek(); s != ']') {
                        while (ifs.peek() != ']') {
                            auto line = std::make_unique<cg::Line>();
                            try {
                                line->_deserialize(ifs);
                            }
                            catch (const std::exception& e) {
                                print_error("Failed to deserialize line: %s", e.what());
                                return;
							}
                            if (ifs.eof()) {
                                print_warning("Unexpected end of file while reading lines.");
                                return;
                            }
                            canvas->insert(std::move(line));
                            while (ifs.peek() == ' ' || ifs.peek() == '\n' || ifs.peek() == '\r')
                                ifs.ignore(); // Ignore leading whitespace
                        }
                        if (ifs.peek() == ']') {
                            ifs.ignore(); // Ignore ']'
                        }
                        else
                            print_warning("Invalid file format. Expected ']' but got '%c', continuing...", ifs.peek());
                    }
                    else
						ifs.ignore(); // empty
                }
			}
			ifs >> dummie; // Read "Polygons:"
            if (dummie != "Polygons:")
                print_warning("Invalid file format. Expected 'Polygons:' but got '%s', continuing...", dummie.c_str());
            else {
                while (ifs.peek() == ' ' || ifs.peek() == '\n' || ifs.peek() == '\r')
                    ifs.ignore(); // Ignore leading whitespace

                if (char s = ifs.peek(); s != '[')
                    print_warning("Invalid file format. Expected '[' but got '%s', continuing...", dummie.c_str());
                else {
                    ifs.ignore(); // Ignore '['
                    while (ifs.peek() == ' ' || ifs.peek() == '\n' || ifs.peek() == '\r')
                        ifs.ignore(); // Ignore leading whitespace

                    if (char s = ifs.peek(); s != ']') {
                        while (dummie != "]") {
                            auto polygon = std::make_unique<cg::Polygon>();
                            try {
                                polygon->_deserialize(ifs);
                            }
                            catch (const std::exception& e) {
                                print_error("Failed to deserialize polygon: %s", e.what());
                                return;
							}
                            if (ifs.eof()) {
                                print_warning("Unexpected end of file while reading polygons.");
                                return;
                            }
                            canvas->insert(std::move(polygon));
                            while (ifs.peek() == ' ' || ifs.peek() == '\n' || ifs.peek() == '\r')
                                ifs.ignore(); // Ignore leading whitespace
                        }
                        if (ifs.peek() == ']') {
                            ifs.ignore(); // Ignore ']'
                        }
                        else
                            print_warning("Invalid file format. Expected ']' but got '%c', continuing...", ifs.peek());
                    }
                    else
						ifs.ignore(); // empty
				}
			}

        });
    }

} // namespace cg
