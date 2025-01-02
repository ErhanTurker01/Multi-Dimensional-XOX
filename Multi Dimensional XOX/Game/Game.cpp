#include "Game.hpp"
#include <iostream>

namespace XOX {

std::ostream& operator<<(std::ostream& os, State state) {
    switch (state) {
        case State::X: os << "X"; break;
        case State::O: os << "O"; break;
        case State::N: os << "None"; break;
        case State::D: os << "Draw"; break;
        default: os << "Unknown"; break;
    }
    return os;
}

} // namespace XOX
