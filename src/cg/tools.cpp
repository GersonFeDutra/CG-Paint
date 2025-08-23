#include "tools.hpp"

#include "canvas.hpp"

namespace cg {

	void Painter::appendToCanvas(CanvasItem* item)
	{
		toolBox.canvas->insert(std::unique_ptr<CanvasItem>(item));
	}

	void Painter::_render()
	{
		if (isDrawing && !toolBox.isInsideGui)
			_onRender();
	}

	void Selector::pick(const std::vector<CanvasItem*>& items, Vector2 mousePos)
	{
		selectedItem = toolBox.canvas->hitTest(mousePos.x, mousePos.y);
	}

}