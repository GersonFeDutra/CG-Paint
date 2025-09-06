#pragma once

#include "cg/math.hpp"
#include <cg/canvas_itens/line.hpp>


namespace cg {

class Gizmo {
public:
    Gizmo(float size = 1.0f, Transform2D* model = nullptr)
            : lines{Line{Line::KeepPivot{}, Vector2::left() * size, Vector2::zero(), colors::BLUE, 2.0f},
                    Line{Line::KeepPivot{}, Vector2::up() * size, Vector2::zero(), colors::GREEN, 2.0f},
                    Line{Line::KeepPivot{}, Vector2::right() * size, Vector2::zero(), colors::RED, 2.0f},
                    Line{Line::KeepPivot{}, Vector2::down() * size, Vector2::zero(), colors::CYAN, 2.0f} } {
        this->model = model;
    }

    void _render() {
        if (model == nullptr)
            return;
        for (Line& line : lines) {
            line.model = *model;
            line._render();
        }
    }

    void attach(Transform2D* model) {
        this->model = model;
    }

private:
    Line lines[4];
    Transform2D* model = nullptr;
};

class GuideLine : public Line {
    friend ToolBox;
public:
    // Assume que a diereção está normalizada
    GuideLine(Vector2 direction, Vector2 window_size, Color color = { 2 / 9.0f, 2 / 9.0f, 2 / 9.0f })
        : Line{ color }, direction{ direction } {
        vertices.push_back(-direction);
        vertices.push_back(direction);
        model = Transform2D(window_size.x, 0, 0, window_size.y);
        noSerialize();
    }
    void _reshape(Canvas& canvas) override {
        auto [x, y] = canvas.getWindowSize();
        model = Transform2D(x, 0, 0, y);
    }
private:
    Vector2 direction;
};

}
