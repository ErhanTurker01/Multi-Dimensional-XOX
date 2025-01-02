#pragma once
#include <array>
#include <vector>
#include <stdexcept>
#include <format>
#include <iostream>

using u32 = uint32_t;
using u8 = uint8_t;
using std::array;
using std::vector;

namespace XOX {

enum class State { X, O, N, D }; /* X, O, None, Draw */

template<u32 N>
class Game {
    friend Game<N+1>;
protected:
    array<array<Game<N-1>, 3>, 3> board;
    Game<N+1>* biggerGame = nullptr;
    State state = State::N;
    
public:
    /* Constructor */
    Game(){
        for(auto& row : board){
            for(Game<N-1>& game : row){
                game.biggerGame = this;
            }
        }
    }
    
};
template<>
class Game<1> {
    friend Game<2>;
protected:
    array<array<State, 3>, 3> board;
    Game<2>* biggerGame = nullptr;
    State state = State::N;

public:
    /* Constructor */
    Game() {
        for (auto& row : board) {
            row.fill(State::N);
        }
    }
    
};

/* State print */
std::ostream& operator<<(std::ostream& os, State state);

}
