#pragma once

#include "cg/math.hpp"
#include <cg/canvas_itens/line.hpp>


namespace cg {

class Gizmo {
public:
    Gizmo(float size, Vector2 at, Color color = { .333, .333, .333 })
            : lines{ Line{Vector2::up() * size, Vector2::down() * size, color}, Line{Vector2::left() * size, Vector2::right() * size, color} } {}

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
    Line lines[2];
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