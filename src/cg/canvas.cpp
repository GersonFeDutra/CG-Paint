#include "canvas.hpp"


namespace cg {
	void Canvas::sendInput(io::MouseLeftButtonPressed input_event)
	{
		current = toolBox.newPrimitive(input_event.position);
	}

	void Canvas::sendInput(io::MouseLeftButtonReleased input_event)
	{
		current = nullptr;
	}

	void Canvas::sendInput(io::MouseDrag input_event)
	{
		if (current)
			current->position = input_event.position;
	}

	TimePoint Canvas::updateProcess(TimePoint lastTime)
	{
		// 1. captura o tempo atual para todos os itens
		auto now = std::chrono::steady_clock::now();

		/* Intervalo (△t s): segundos desde o último quadro. */
		DeltaTime delta = std::chrono::duration<float>(now - lastTime).count();

		for (auto& item : itens)
			item->_process(delta);
		return now; // próximo lastTime
	}

	void Canvas::updateRender()
	{
		for (auto& item : itens)
			item->_render();
	}

}
