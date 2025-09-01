#pragma once

#include <array>

#include "math.hpp"
#include "input_event.hpp"


namespace cg {
	// Foward Declarations
	class Canvas;
	class Painter;
	class PointTool;
	class LineTool;
	class SelectTool;

	/** Gerencia as ferramentas atuais usadas no Canvas.
	 * Simplesmente delega os eventos de entrada para a ferramenta atual.
	 */
	class ToolBox {
		friend class Tool;
		static constexpr std::size_t N_PRIMITIVES = 4; // Altere aqui o tamanho do array para adicionar mais ferramentas.
	public:
		ToolBox();
		~ToolBox();

		void addCanvas(Canvas* canvas_ptr);

		void _render();
		void _reshape(Canvas& canvas);

		template <typename IE> requires std::is_base_of_v<io::MouseInputEvent, IE>
		void captureInput(IE input_event) {}

		void captureInput(io::MouseMove input_event);
		void captureInput(io::MouseDrag input_event);
		void captureInput(io::MouseLeftButtonPressed input_event);
		void captureInput(io::MouseLeftButtonReleased input_event);
		void captureInput(io::MouseRightButtonPressed input_event);

		void captureInput(io::SpecialKeyInputEvent input_event);
		void captureInput(io::KeyboardInputEvent input_event);

		void save();
		void load();

		inline Color getColor() const {
			return *colorPtr;
		}
		inline Color* getColorPtr() {
			return colorPtr;
		}
		inline void bindColorPtr(Color* to) {
			assert_err(to != nullptr, "Must not bind to a nullptr");
			colorPtr = to;
		}
		inline void unbindColorPtr() {
			warn(colorPtr == &currentColor, "color was not bind before");
			currentColor = *colorPtr;
			colorPtr = &currentColor;
		}

		inline SelectTool& getSelectorTool() {
			return *(SelectTool *)tools[Tools::SELECT];
		}

	public:
		// TODO -> Cor secundária [alternada com x key]
		Canvas* canvas = nullptr;
		bool isInsideGui = false;
    private:
		enum Tools {
			POINT = 0,
			LINE = 1,
			POLYGON = 2,
			SELECT = 3,
		};
	private:
		int currentTool = POINT;
		std::array<Painter *, N_PRIMITIVES> tools;

		Color currentColor = cg::colors::WHITE;
		Color *colorPtr = &currentColor; // Define a cor atual para pintura.
	};

}