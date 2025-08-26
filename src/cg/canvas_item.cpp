#include "canvas_item.hpp"

namespace cg {

    std::ostream& operator<<(std::ostream& os, const CanvasItem& item) {
        return item._print(os);;
    }

    std::istream& operator>>(std::istream& is, CanvasItem& item) {
        return item._read(is);
    }

    std::ofstream& operator<<(std::ofstream& ofs, const CanvasItem& item) {
        return item._serialize(ofs);
    }

    std::ifstream& operator>>(std::ifstream& ifs, CanvasItem& item) {
        return item._deserialize(ifs);
    }

    std::istream& CanvasItem::_read(std::istream& is) { return is; }
}