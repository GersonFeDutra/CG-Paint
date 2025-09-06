#include "canvas_item.hpp"

namespace cg {

    std::ostream& operator<<(std::ostream& os, const CanvasItem& item) {
        return item._serialize(os);
    }

    std::istream& operator>>(std::istream& is, CanvasItem& item) {
        return item._deserialize(is);
    }
}
