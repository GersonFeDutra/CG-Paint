#pragma once

#include "util.hpp"
#include <vector>

#include <cg/canvas.hpp>

#include <cg/math.hpp>
#include <cg/geometry.hpp>


namespace cg
{
    class Line : public CanvasItem
    {
        friend class Gizmo;
        inline static Vector2 selectionOffset{};
    public:
        Line() : CanvasItem{ TypeInfo::LINE } {
            vertices.reserve(2);
		}

        Line(Color color) : CanvasItem{ TypeInfo::LINE }, color{ color } {
            vertices.reserve(2);
        }

        Line(Vector2 position, Color color = Color{})
                : CanvasItem{ TypeInfo::LINE, position }, color{ color }
        {
            vertices.reserve(2);
			vertices.emplace_back(Vector2{}); // O ponto inicial é a origem local
        }

        Line(Vector2 from, Vector2 to, Color color = Color{}, float width = 1.0f)
                : CanvasItem{ TypeInfo::LINE, from }, color{ color }, width{width}
        {
            vertices.reserve(2);
            vertices.emplace_back(Vector2{});
            vertices.emplace_back(toLocal(to)); // Os pontos da linha são relativos às coordenadas locais
            setPivotToMiddle();
        }

        struct KeepPivot{};

        Line(KeepPivot _, Vector2 from, Vector2 to, Color color = Color{}, float width = 1.0f)
                : CanvasItem{ TypeInfo::LINE, from }, color{ color }, width{width}
        {
            vertices.reserve(2);
            vertices.emplace_back(Vector2{});
            vertices.emplace_back(toLocal(to)); // Os pontos da linha são relativos às coordenadas locais
        }

        //void _process(DeltaTime delta) override;

        void _render() override;

        void _input(io::MouseLeftButtonPressed mouse_event) {
            // Calcula o offset em relação ao pivôt
            Line::selectionOffset = mouse_event.position - model.getOrigin();
        }

        void _input(io::MouseDrag mouse_event) {
            // Aplica a translação ajustada sobre o pivôt
            translateTo(mouse_event.position - selectionOffset);
        }

        // Verifica se a linha foi selecionada pelo mouse
        bool _isSelected(Vector2 cursor_local_position) const override;

        inline void append(Vector2 vertice) {
            // Armazena o ponto relativo ao sistema de coordenadas local do modelo
            vertices.push_back(toLocal(vertice));
			setPivotToMiddle(); // Atualiza o sistema de coordenadas local
        }

		// Define o pivô como o ponto médio entre todos os vértices
        void setPivotToMiddle() {
            Vector2 middle{};
            for (const auto& vertice : vertices)
                middle += model * vertice;
            setPivot(middle / vertices.size());
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

        // Tamanho da corda, desconsiderando vértice de origem.
        inline size_t size() const {
            return vertices.size();
        }

        inline Vector2 lastVertice() const {
			assert_err(size() > 0, "No vertices in line");
            return toGlobal(vertices.back());
        }

		// FIXME: essa função pode ser perigosa, pois permite modificar os vertices diretamente
        // Guido: adicionei essa função pra ver se um ngc funciona no polygon_tool
        inline std::vector<Vector2> getVertices() {
            return vertices;
        }

        inline Color& getColor() {
            return color;
        }

        inline Color getColor() const {
            return color;
        }

        inline void setColor(ColorRgb lineColor) {
            color = lineColor;
        }

        inline void setVertices(std::vector<Vector2> lineVertices) {
            vertices = lineVertices;
        }

        // Inherited via CanvasItem
        std::ostream& _print(std::ostream& os) const override;
        std::ofstream& _serialize(std::ofstream& ofs) const override;
        std::ifstream& _deserialize(std::ifstream& ifs) override;
    protected:
        std::vector<Vector2> vertices;
        Color color{}; // TODO -> alpha blending
		float width = 1.0f; // TODO -> anti-alias
    };

} // namespace cg
