#include <string>
#include <sstream>

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
#include "tools/gizmo.hpp"


namespace cg {

	// Guide lines
	GuideLine* guideLines[2] = { nullptr, nullptr };

	ToolBox::ToolBox() : tools{ nullptr, nullptr, nullptr, nullptr } {}

	ToolBox::~ToolBox()
	{
		for (auto*& guide : guideLines) {
			delete guide;
			guide = nullptr;
		}
	}

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

		guideLines[0] = new GuideLine(Vector2::down(), canvas->getWindowSize() / 2.0f);
		guideLines[1] = new GuideLine(Vector2::right(), canvas->getWindowSize() / 2.0f);

		for (auto* line : guideLines)
			for (const auto &vertice : line->getVertices())
				std::cout << 'v' << vertice << '\n';
		// Will be destroyed by ToolBox
	}

	static unsigned tool_cursor = GLUT_CURSOR_INHERIT;
	static unsigned last_cursor = GLUT_CURSOR_INHERIT;

	static inline bool is_scale_inside_bounds(Vector2 scale, Vector2 increment) {
		return !(increment < Vector2::zero() && scale < Vector2(1e-3f) + increment ||
			increment > Vector2::zero() && scale > Vector2(16.0f) - increment);
	}

	void ToolBox::_render()
	{
		// Cada janela deve ser criada num escopo separado para
		// invocar os construtores/ destrutores necessários
		{
			Window settings("Settings");

			static bool showGuideLines = true;
			if (settings.showCheckBox(&showGuideLines, "Show Guide Lines")) {}
			if (showGuideLines)
				for (auto* guide : guideLines)
					guide->_render();

			settings.showText("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / Gui::getFps(), Gui::getFps());
		}

		enum { NONE, SAVE, LOAD } clicked = NONE;
		{  // Mostra uma janela simples.
			static int counter = 0;
			static float f = 0.0f;

			Window toolBox("ToolBox");
			// TODO -> Use Icon buttons for each tool

			// toolBox.showCheckBox(&check, "Active

			// glut cursor
			unsigned current_cursor = GLUT_CURSOR_INHERIT;

			int _currentTool = currentTool;
			if (toolBox.showRadioButton(&_currentTool, Tools::POINT, "Point [F1]")) {
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

			// Update translation
			{
				Vector2 translation = tools[Tools::SELECT]->getPosition();
				cg::Vector2 increment = toolBox.showIncrementalSliderVector2(
					&translation,
					canvas->getUpperLeft(),
					canvas->getBottomRight(),
					1.0f,
					"X", "<", ">",
					"Y", "V", "^"
				);

				if (increment) {
					tools[Tools::SELECT]->translate(increment);
				}
				else {
					if (translation != tools[Tools::SELECT]->getPosition())
						tools[Tools::SELECT]->setPosition(translation);
				}
				//toolBox.showSliderVector2(&translation, canvas->getUpperLeft(), canvas->getBottomRight(), "X", "Y");

			}
			// Update rotation
			{
				float rotation_deg = rad_to_deg(tools[Tools::SELECT]->getRotation()); // degrees
				int increment_unit = toolBox.showIncrementalFloatSlider(
					&rotation_deg, -180.0f, 180.0f, 1.0f, "Rotation", "-1 deg", "+1 deg", "[", "]");
				
				if (increment_unit) // Aply negative / positive rotation
					tools[Tools::SELECT]->rotate(deg_to_rad((float)increment_unit));
				else {
					float rotation_rad = deg_to_rad(rotation_deg); // radians
					if (fabsf(rotation_rad - tools[Tools::SELECT]->getRotation()) > ZERO_PRECISION_ERROR)
						tools[Tools::SELECT]->setRotation(rotation_rad);
				}
			}
			// Update scale
			{
				// FIXME
				Vector2 scale = tools[Tools::SELECT]->getScale();
				Vector2 increment = toolBox.showIncrementalSliderVector2(&scale, 1e-3f, 16.0f, .1f,
					"Sx", "*.9x", "*1.1x", "Sy", "*.9y", "*1.1y");
				//toolBox.showSliderVector2(&scale, 1e-3f, 16.0f, "Sx", "Sy");

				if (increment) {
					if (is_scale_inside_bounds(tools[Tools::SELECT]->getScale(), increment))
						tools[Tools::SELECT]->scale(Vector2::one() + increment);
				}
				else if ((scale - tools[Tools::SELECT]->getScale()).abs() > Vector2(ZERO_PRECISION_ERROR))
					tools[Tools::SELECT]->setScale(scale);
			}
			// Cisalhamento
			{
				// TODO
			}

			// Save / Load
			{
				if (toolBox.showButton("Save to file"))
					clicked = SAVE;
				toolBox.sameLine();
				if (toolBox.showButton("Load from file"))
					clicked = LOAD;
			}
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

	void ToolBox::_reshape(Canvas& canvas)
	{
		for (auto* guides : guideLines)
			guides->_reshape(canvas);
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
		switch (input_event.mods) {
		case GLUT_ACTIVE_SHIFT:
			switch (input_event.key) {
			case GLUT_KEY_RIGHT: {
				if (is_scale_inside_bounds(tools[Tools::SELECT]->getScale(), Vector2::right()))
					tools[Tools::SELECT]->scale(Vector2::one() + Vector2::right() * .1f);
			} break;
			case GLUT_KEY_LEFT: {
				if (is_scale_inside_bounds(tools[Tools::SELECT]->getScale(), Vector2::left()))
					tools[Tools::SELECT]->scale(Vector2::one() + Vector2::left() * .1f);
			} break;
			case GLUT_KEY_UP: {
				if (is_scale_inside_bounds(tools[Tools::SELECT]->getScale(), Vector2::up()))
					tools[Tools::SELECT]->scale(Vector2::one() + Vector2::up() * .1f);
			} break;
			case GLUT_KEY_DOWN: {
				if (is_scale_inside_bounds(tools[Tools::SELECT]->getScale(), Vector2::down()))
					tools[Tools::SELECT]->scale(Vector2::one() + Vector2::down() * .1f);
			} break;
			default:
				break;
			}
			break;
		default:
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
			case GLUT_KEY_RIGHT:
				tools[Tools::SELECT]->translate(Vector2::right());
				break;
			case GLUT_KEY_LEFT:
				tools[Tools::SELECT]->translate(Vector2::left());
				break;
			case GLUT_KEY_UP:
				tools[Tools::SELECT]->translate(Vector2::up());
				break;
			case GLUT_KEY_DOWN:
				tools[Tools::SELECT]->translate(Vector2::down());
				break;
			default: // ignore
				break;
			}
			break;
		}
	}

	void ToolBox::captureInput(io::KeyboardInputEvent input_event)
	{
		const unsigned char ESC = 27;

		switch (input_event.key) {
		case '[':
			tools[Tools::SELECT]->rotate(deg_to_rad(-1.0f));
			break;
		case ']':
			tools[Tools::SELECT]->rotate(deg_to_rad(1.0f));
			break;
		default:
			break;
		}

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
