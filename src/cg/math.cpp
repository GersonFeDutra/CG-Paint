#include "math.hpp"

namespace cg {

// std::ofstream& operator<<(std::ofstream& ofs, Color c)
// {
//     ofs << '(' << c.r << ", " << c.g << ", " << c.b << ", " << c.a << ')';  // Save in the format (r, g, b, a)
//     return ofs;
// }

// std::ifstream& operator>>(std::ifstream& ifs, Color& c)
// {
//     try {
//         while (ifs.peek() == ' ' || ifs.peek() == '\n' || ifs.peek() == '\r')
//             ifs.ignore(); // Ignore leading whitespace
// 
//         Color parsed;
//         // Load in the format (x, y)
//         if (ifs.peek() == '(')
//             ifs.ignore(); // Ignore '('
//         else
//             ifs.setstate(std::ios::failbit);
// 
//         // Read red
//         if (!(ifs >> parsed.r)) {
//             ifs.setstate(std::ios::failbit); // marca falha no stream
//             return ifs;
//         }
// 
//         while (ifs.peek() == ' ' || ifs.peek() == '\n' || ifs.peek() == '\r')
//             ifs.ignore(); // Ignore leading whitespace
//         if (ifs.peek() == ',')
//             ifs.ignore(); // Ignore ','
//         else
//             ifs.setstate(std::ios::failbit);
// 
//         // Read green
//         if (!(ifs >> parsed.g)) {
//             ifs.setstate(std::ios::failbit); // marca falha no stream
//             return ifs;
//         }
// 
//         while (ifs.peek() == ' ' || ifs.peek() == '\n' || ifs.peek() == '\r')
//             ifs.ignore(); // Ignore leading whitespace
//         if (ifs.peek() == ',')
//             ifs.ignore(); // Ignore ','
//         else
//             ifs.setstate(std::ios::failbit);
// 
//         // Read blue
//         if (!(ifs >> parsed.b)) {
//             ifs.setstate(std::ios::failbit); // marca falha no stream
//             return ifs;
//         }
// 
//         while (ifs.peek() == ' ' || ifs.peek() == '\n' || ifs.peek() == '\r')
//             ifs.ignore(); // Ignore leading whitespace
//         if (ifs.peek() == ',')
//             ifs.ignore(); // Ignore ','
//         else
//             ifs.setstate(std::ios::failbit);
// 
//         // Read alpha
//         if (!(ifs >> parsed.a)) {
//             ifs.setstate(std::ios::failbit); // marca falha no stream
//             return ifs;
//         }
//         while (ifs.peek() == ' ' || ifs.peek() == '\n' || ifs.peek() == '\r')
//             ifs.ignore(); // Ignore leading whitespace
//         if (ifs.peek() == ')')
//             ifs.ignore(); // Ignore ')'
//         else
//             ifs.setstate(std::ios::failbit);
// 
//         c = parsed; // Assign parsed values to c
//     }
//     catch (...) {
//         ifs.setstate(std::ios::failbit);
//     }
//     return ifs;
// }

std::ostream& operator<<(std::ostream& os, Color c)
{
    return os << "Color( " << c.r << ' ' << c.g << ' ' << c.b << ' ' << c.a << " )";
}

std::istream& operator>>(std::istream& is, Color& c)
{
    try {
        std::string dummy;
        Color parsed;
        if (!(is >> dummy >> parsed.r >> parsed.g >> parsed.b >> parsed.a) || dummy != "Color(" ||
            !(is >> dummy) || dummy != ")") { // Read in the format Color(r g b a)
            is.setstate(std::ios::failbit); // marca falha no stream
            return is;
        } 
        c = parsed;
    }
    catch (...) {
        is.setstate(std::ios::failbit);
    }
    return is;
}
}
