#pragma once
#include <array>
#include <vector>
#include <stdexcept>
#include <format>
#include <iostream>
#include <type_traits>

using u32 = uint32_t;
using u8 = uint8_t;

namespace XOX {

const u8 MAX_DEPTH = 5;
const u8 MIN_SIZE = 3;

enum class State { X, O, N, D }; /* X, O, None, Draw */

template<u32 Depth, u32 Size = 3>
class Game {
    static_assert(Depth <= MAX_DEPTH, "Game is too deep!");
    static_assert(Size >= MIN_SIZE, "Game is too small!");
    
    friend Game<Depth+1, Size>;
    using Type = typename std::conditional<Depth==1, State, Game<Depth-1, Size>>::type;
    std::array<std::array<Type, Size>, Size> board;
    Game<Depth+1, Size>* biggerGame = nullptr;
    State state = State::N;
    
public:
    Game();
    operator State() const;
    typename std::array<Type, Size>& operator[](u8 i);
    Type& element(const std::vector<u8>& indexes, size_t offset = 0);
    void updateWinner();
};


template<u32 Depth, u32 Size>
Game<Depth, Size>::operator State() const{
    return state;
}

template<u32 Depth, u32 Size>
Game<Depth, Size>::Game() {
    for (auto& row : board) {
        if constexpr (Depth == 1) row.fill(State::N);
        else for (Game<Depth-1, Size>& game : row) game.biggerGame = this;
    }
}

template<u32 Depth, u32 Size>
typename std::array<typename Game<Depth, Size>::Type, Size>&
Game<Depth, Size>::operator[](u8 i) {
    return board[i];
}

template<u32 Depth, u32 Size>
typename Game<Depth, Size>::Type&
Game<Depth, Size>::element(const std::vector<u8>& indexes, size_t offset) {
    if (indexes.size() - offset != Depth * 2) {
        std::string error = std::format("Indexes size is not {} for Game<{}, {}>!", Depth * 2, Depth, Size);
        throw std::invalid_argument(error);
    }

    if constexpr (Depth == 1) {
        return board[indexes[offset]][indexes[offset + 1]];
    } else {
        return board[indexes[offset]][indexes[offset + 1]].element(indexes, offset + 2);
    }
}

template<u32 Depth, u32 Size>
void Game<Depth, Size>::updateWinner() {
    for (int i = 0; i < Size; ++i) {
        if (std::all_of(board[i].begin(), board[i].end(), [&](State s) { return s == board[i][0] && s != State::N; })) {
            state = board[i][0];
            if constexpr (Depth < MAX_DEPTH)
                if (biggerGame) biggerGame->updateWinner();
            return;
        }
        if (std::all_of(board.begin(), board.end(), [&](auto& row) { return row[i] == board[0][i] && row[i] != State::N; })) {
            state = board[0][i];
            if constexpr (Depth < MAX_DEPTH)
                if (biggerGame) biggerGame->updateWinner();
            return;
        }
    }

    if (std::all_of(board.begin(), board.end(), [&](auto& row) { return row[&row - board.begin()] == board[0][0] && row[&row - board.begin()] != State::N; })) {
        state = board[0][0];
        if constexpr (Depth < MAX_DEPTH)
            if (biggerGame) biggerGame->updateWinner();
        return;
    }
    if (std::all_of(board.begin(), board.end(), [&](auto& row) { return row[Size - 1 - (&row - board.begin())] == board[0][Size - 1] && row[Size - 1 - (&row - board.begin())] != State::N; })) {
        state = board[0][Size - 1];
        if constexpr (Depth < MAX_DEPTH)
            if (biggerGame) biggerGame->updateWinner();
        return;
    }

    for (const auto& row : board) {
        if (std::any_of(row.begin(), row.end(), [](State s) { return s == State::N; })) {
            state = State::N;
            return;
        }
    }
    
    state = State::D;
    if constexpr (Depth < MAX_DEPTH)
        if (biggerGame) biggerGame->updateWinner();
}


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

}
