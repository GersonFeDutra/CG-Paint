#include "canvas.hpp"


namespace cg {

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