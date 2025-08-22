#include "math.hpp"

namespace cg {

    std::ostream& operator<<(std::ostream& os, Color c)
    {
        return os << "Color(" << c.r << ", " << c.g << ", " << c.b << ", " << c.a << ")";
    }

    std::istream& operator>>(std::istream& is, Color& c)
    {
        try {
            std::string dummy;
            char s;
            if (!(is >> dummy >> c.r >> s >> c.g >> s >> c.b >> s >> c.a >> s)) // Read in the format Color(r, g, b, a)
                is.setstate(std::ios::failbit); // marca falha no stream
        }
        catch (...) {
            is.setstate(std::ios::failbit);
        }
        return is;
    }

    std::ofstream& operator<<(std::ofstream& ofs, Color c)
    {
        ofs << '(' << c.r << ", " << c.g << ", " << c.b << ", " << c.a << ')';  // Save in the format (r, g, b, a)
        return ofs;
    }

    std::ifstream& operator>>(std::ifstream& ifs, Color& c)
    {
        try {
            char s;
            if (!(ifs >> s >> c.r >> s >> c.g >> s >> c.b >> s >> c.a >> s))  // Load in the format (r, g, b, a)
                ifs.setstate(std::ios::failbit); // marca falha no stream
        }
        catch (...) {
            ifs.setstate(std::ios::failbit);
        }
        return ifs;
    }

} // namespace cg