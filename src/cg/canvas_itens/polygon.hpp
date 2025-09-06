#pragma once

#include <vector>

#include <cg/canvas.hpp>
#include <cg/math.hpp>

#include "../canvas_item.hpp"


namespace cg
{
    class Polygon : public CanvasItem
    {
        inline static Vector2 selectionOffset{};
    public:
        Polygon() : CanvasItem(TypeInfo::POLYGON) {}
        Polygon(Vector2 position, Color color = Color{}) : CanvasItem{ TypeInfo::POLYGON }, innerColor{ color }, contourColor{ color } {
			vertices.reserve(3); // reserva espaço para 3 vértices
            vertices.emplace_back(position);
        }

        Polygon(std::vector<Vector2> verts) : CanvasItem{ TypeInfo::POLYGON }, vertices{ verts } {
            setPivotToMiddle();
        }

        void _render() override;

        void _input(io::MouseLeftButtonPressed mouse_event) {
            // Calcula o offset em relação ao pivôt
            Polygon::selectionOffset = mouse_event.position - model.getOrigin();
        }

        void _input(io::MouseDrag mouse_event) {
            // Aplica a translação ajustada sobre o pivôt
            translateTo(mouse_event.position - selectionOffset);
        }

        // Verifica se o polígono foi selecionado pelo mouse
        bool _isSelected(Vector2 mousePos) const override;

        inline void append(Vector2 newVertex) {
            vertices.push_back(toLocal(newVertex));
            setPivotToMiddle();
        }

        inline void setPivot(Vector2 global_position) {
            // guarda o modelo atual (com rotação + translação antiga)
            Transform2D oldModel = model;

            // se não houver vértices, só movemos o modelo e retornamos
            if (vertices.empty()) {
                model.setOrigin(global_position);
                return;
            }

            // move o pivô (mantendo rotação) — altera apenas a coluna de translação.
            model.setOrigin(global_position);

            // calcula inverso do novo modelo para converter posições globais -> novo sistema local
            Transform2D invNewModel = model.inverse();

            // para cada vértice: pega posição global (usando oldModel) e converte para a nova
            for (auto& v : vertices) {
                Vector2 worldPos = oldModel * v; // posição global do vértice antes da mudança
                v = invNewModel * worldPos;      // nova coordenada local (no sistema com novo pivô)
            }
        }

        // Define o pivô como o ponto médio entre todos os vértices
        void setPivotToMiddle() {
            Vector2 middle{};
            for (const auto& vertice : vertices)
                middle += model * vertice;
            setPivot(middle / vertices.size());
        }

		// Number of vertices
        inline size_t size() {
            return vertices.size();
        }

        inline Vector2& firstVertice() {
            assert_err(!vertices.empty(), "No vertices in polygon.");
            return vertices.front();
        }

        inline Vector2& lastVertice() {
			assert_err(!vertices.empty(), "No vertices in polygon.");
            return vertices.back();
        }

        inline Vector2 lastVertice() const {
            assert_err(!vertices.empty(), "No vertices in polygon.");
            return vertices.back();
        }

		// Returns a copy of the vertices
        inline std::vector<Vector2> getVertices() {
            return vertices;
        }

        inline void setVertices(std::vector<Vector2> allVertices) {
            vertices = allVertices;
        }

        inline void setColor(ColorRgb color) {
            innerColor = color;
        }

        inline Color& getColor() {
            return innerColor;
        }

        inline Color getColor() const {
            return innerColor;
        }

        // Inherited via CanvasItem
        std::ostream& _serialize(std::ostream& os) const override;
        std::istream& _deserialize(std::istream& is) override;
    private:
        std::vector<Vector2> vertices;
        Color innerColor{};
        Color contourColor{}; // TODO -> Implement contour color
		float width = 1.0f; // TODO -> Implement line width
        // TODO -> anti-alias

    };

} // namespace cg
