#include "triangulation.hpp"

#include <cmath>
#include <cstring>
#include <iostream>


namespace cg {

    // thread_local definition
    thread_local Triangulation* Triangulation::s_current_ = nullptr;

    Triangulation::Triangulation()
        : lastHash_(0), valid_(false), lastQuantStep_(1e-3f) {
    }

    Triangulation::~Triangulation() {
        // garantir limpeza
        for (auto p : allocated_) delete p;
        allocated_.clear();
    }

    /* ----------------- Hash / Quantize helpers ----------------- */

    uint64_t Triangulation::fnv1a64(const void* data, size_t len) {
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);
        uint64_t h = 14695981039346656037ULL; // FNV offset basis
        for (size_t i = 0; i < len; ++i) {
            h ^= bytes[i];
            h *= 1099511628211ULL; // FNV prime
        }
        return h;
    }

    void Triangulation::quantizePolygon(const std::vector<Vector2>& pts, std::vector<int64_t>& outQuant, float quant) {
        outQuant.clear();
        outQuant.reserve(1 + pts.size() * 2);
        outQuant.push_back(static_cast<int64_t>(pts.size()));
        const double scale = 1.0 / static_cast<double>(quant);
        for (const auto& p : pts) {
            int64_t xi = std::llround(static_cast<double>(p.x) * scale);
            int64_t yi = std::llround(static_cast<double>(p.y) * scale);
            outQuant.push_back(xi);
            outQuant.push_back(yi);
        }
    }

    uint64_t Triangulation::hashQuantized(const std::vector<int64_t>& q) {
        return fnv1a64(q.data(), q.size() * sizeof(int64_t));
    }

    /* ----------------- GLU tess callbacks ----------------- */

    void CALLBACK Triangulation::beginCB(GLenum /*type*/) {
        // nada necessário aqui (nós coletamos índices em vertexCB)
    }

    void CALLBACK Triangulation::vertexCB(const GLvoid* data) {
        const TessVertex* tv = reinterpret_cast<const TessVertex*>(data);
        if (s_current_) {
            s_current_->triIndices_.push_back(tv->idx);
        }
    }

    void CALLBACK Triangulation::endCB() {
        // noop
    }

    void CALLBACK Triangulation::errorCB(GLenum errCode) {
        const char* str = reinterpret_cast<const char*>(gluErrorString(errCode));
        std::cerr << "[Triangulation] GLU tess error: " << (str ? str : "Unknown") << std::endl;
    }

    void CALLBACK Triangulation::combineCB(GLdouble coords[3], void* /*vertex_data*/[4], GLfloat /*weight*/[4], void** outData) {
        // Criar um novo TessVertex, adicionar ao outVerts e retornar pointer
        if (!s_current_) {
            *outData = nullptr;
            return;
        }
        TessVertex* nv = new TessVertex();
        nv->coord[0] = coords[0];
        nv->coord[1] = coords[1];
        nv->coord[2] = coords[2];
        // salva para liberar depois
        s_current_->allocated_.push_back(nv);

        // adiciona também ao triVerts_ e define idx
        s_current_->triVerts_.emplace_back(static_cast<float>(coords[0]), static_cast<float>(coords[1]));
        nv->idx = static_cast<int>(s_current_->triVerts_.size() - 1);

        *outData = nv;
    }

    /* ----------------- Run GLU tessellator ----------------- */

    bool Triangulation::runGluTessellate(const std::vector<Vector2>& contour) {
        triVerts_.clear();
        triIndices_.clear();
        // liberamos alocações anteriores (se houver)
        for (auto p : allocated_) delete p;
        allocated_.clear();

        if (contour.size() < 3) return false;

        // preparar pontos iniciais (TessVertex) e triVerts_ com os pontos originais
        std::vector<TessVertex*> initial;
        initial.reserve(contour.size());
        for (size_t i = 0; i < contour.size(); ++i) {
            TessVertex* tv = new TessVertex();
            tv->coord[0] = static_cast<GLdouble>(contour[i].x);
            tv->coord[1] = static_cast<GLdouble>(contour[i].y);
            tv->coord[2] = 0.0;
            tv->idx = static_cast<int>(triVerts_.size());
            triVerts_.emplace_back(contour[i].x, contour[i].y);
            initial.push_back(tv);
            allocated_.push_back(tv);
        }

        // prepare context pointer para callbacks
        s_current_ = this;

        GLUtesselator* tess = gluNewTess();
        if (!tess) {
            std::cerr << "[Triangulation] gluNewTess failed\n";
            // cleanup already allocated
            for (auto p : allocated_) delete p;
            allocated_.clear();
            s_current_ = nullptr;
            return false;
        }

        // Register callbacks
        gluTessCallback(tess, GLU_TESS_BEGIN, (void (CALLBACK*)()) & Triangulation::beginCB);
        gluTessCallback(tess, GLU_TESS_VERTEX, (void (CALLBACK*)()) & Triangulation::vertexCB);
        gluTessCallback(tess, GLU_TESS_END, (void (CALLBACK*)()) & Triangulation::endCB);
        gluTessCallback(tess, GLU_TESS_ERROR, (void (CALLBACK*)()) & Triangulation::errorCB);
        gluTessCallback(tess, GLU_TESS_COMBINE, (void (CALLBACK*)()) & Triangulation::combineCB);

        gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);

        // Execute tessellation
        gluTessBeginPolygon(tess, nullptr);
        gluTessBeginContour(tess);
        for (TessVertex* v : initial) {
            gluTessVertex(tess, v->coord, v); // pass TessVertex* as vertex data
        }
        gluTessEndContour(tess);
        gluTessEndPolygon(tess);

        // cleanup
        gluDeleteTess(tess);

        // free temporary TessVertex objects (we already copied final verts into triVerts_, combine-created verts also pushed there)
        for (TessVertex* p : allocated_) delete p;
        allocated_.clear();

        s_current_ = nullptr;

        // triIndices_ filled inside vertexCB
        return !triIndices_.empty();
    }

    /* ----------------- Public API ----------------- */

    void Triangulation::invalidate() {
        lastHash_ = 0;
        lastQuant_.clear();
        triVerts_.clear();
        triIndices_.clear();
        valid_ = false;
    }

    bool Triangulation::triangulateIfNeeded(const std::vector<Vector2>& contour, float quant) {
        lastQuantStep_ = quant;
        // special cases handled by caller, but handle here too
        if (contour.size() < 3) {
            // cache simple copy for fallback
            triVerts_ = contour;
            triIndices_.clear();
            lastHash_ = 0;
            lastQuant_.clear();
            valid_ = true;
            return true;
        }

        // quantize + hash
        std::vector<int64_t> q;
        quantizePolygon(contour, q, quant);
        uint64_t h = hashQuantized(q);

        if (valid_ && h == lastHash_) {
            if (lastQuant_ == q) {
                // identical, reuse
                return true;
            }
            // hash collision or quantization difference -> re-tess
        }

        // need to retessellate
        bool ok = runGluTessellate(contour);
        // update cache even if tess failed (so we don't loop forever); if fail we still store lastHash/lastQuant
        lastHash_ = h;
        lastQuant_.swap(q);
        valid_ = ok || (!ok && !triIndices_.empty());
        return valid_;
    }

} // namespace cg
