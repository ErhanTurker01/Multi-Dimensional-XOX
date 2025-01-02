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
    
    /* Update the winner if there is one and also update all the bigger games */
    void updateWinner(){}
    
    /* [] operator */
    array<Game<N-1>, 3>& operator[](u8 i) {
        return board[i];
    }
    
    /* Vector element indexing */
    decltype(auto) element(const vector<u8>& indexes, size_t offset = 0) {
        if (indexes.size() - offset != N * 2) {
            std::string error = std::format("Indexes size is not {} for Game<{}>!", N * 2, N);
            throw std::invalid_argument(error);
        }
        return board[indexes[offset]][indexes[offset + 1]].element(indexes, offset + 2);
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
    
    /* Update the winner if there is one and also update all the bigger games */
    void updateWinner() {
        for (int i = 0; i < 3; ++i) {
            State first = board[i][0];
            if (first != State::N && board[i][1] == first && board[i][2] == first) {
                state = first;
                if (biggerGame) biggerGame->updateWinner();
                return;
            }
        }

        for (int i = 0; i < 3; ++i) {
            State first = board[0][i];
            if (first != State::N && board[1][i] == first && board[2][i] == first) {
                state = first;
                if (biggerGame) biggerGame->updateWinner();
                return;
            }
        }

        State first = board[0][0];
        if (first != State::N && board[1][1] == first && board[2][2] == first) {
            state = first;
            if (biggerGame) biggerGame->updateWinner();
            return;
        }

        first = board[0][2];
        if (first != State::N && board[1][1] == first && board[2][0] == first) {
            state = first;
            if (biggerGame) biggerGame->updateWinner();
            return;
        }

        bool isDraw = true;
        for (const auto& row : board) {
            for (State cell : row) {
                if (cell == State::N) {
                    isDraw = false;
                    break;
                }
            }
            if (!isDraw) break;
        }

        state = isDraw ? State::D : State::N;
    }

    
    /* [] operator */
    array<State, 3>& operator[](u8 i) {
        return board[i];
    }
    
    /* Vector element indexing */
    State& element(const vector<u8>& indexes, size_t offset = 0) {
        if (indexes.size() - offset != 2) {
            throw std::invalid_argument("Indexes size is not 2 for Game<1>!");
        }
        return board[indexes[offset]][indexes[offset + 1]];
    }
};

/* State print */
std::ostream& operator<<(std::ostream& os, State state);

}
