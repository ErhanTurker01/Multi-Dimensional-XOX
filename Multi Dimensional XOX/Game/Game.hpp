#pragma once
#include <array>
#include <vector>
#include <stdexcept>
#include <format>
#include <iostream>
#include <type_traits>
#include <cstdint>
#include <algorithm>
#include <initializer_list>
#include <utility>

using std::size_t;

namespace XOX {

// Constants and allowed ranges.
const size_t MAX_DEPTH = 5;
const size_t MIN_SIZE  = 3;
const size_t MAX_DIM   = 4;
const size_t MIN_DIM   = 2;

enum class State { X, O, N, D };  // X, O, None, Draw

template<typename T, size_t N, size_t Size>
class NArray {
public:
    using type = std::array<typename NArray<T, N-1, Size>::type, Size>;
};

template<typename T, size_t Size>
class NArray<T, 1, Size> {
public:
    using type = std::array<T, Size>;
};

template<typename Container, typename Index, typename... Indices>
decltype(auto) getElement(Container& container, Index idx, Indices... indices) {
    if constexpr (sizeof...(indices) == 0) {
        return container[idx];
    } else {
        return getElement(container[idx], indices...);
    }
}

template<size_t Depth, size_t Dimension, size_t Size>
class Game {
    static_assert(Depth <= MAX_DEPTH, "Game is too deep!");
    static_assert(Size >= MIN_SIZE, "Game is too small!");
    static_assert(Dimension >= MIN_DIM, "Game is too small!");
    static_assert(Dimension <= MAX_DIM, "Game is too big!");
    
    friend Game<Depth+1, Dimension, Size>;
    
public:
    using stateType = typename std::conditional<Depth == 1, State, Game<Depth-1, Dimension, Size>>::type;
    using boardType = typename NArray<stateType, Dimension, Size>::type;
    
private:
    boardType board;
    Game<Depth+1, Dimension, Size>* biggerGame = nullptr;
    State state = State::N;

    template<typename T>
    void initialize(T& container);
    
    template<std::size_t N, std::size_t... I>
    auto& element_imp(const std::array<size_t, N>& idx, std::index_sequence<I...>);
    
public:
    Game();
    operator State() const;
    auto& operator[](size_t i);
    
    auto& element(std::initializer_list<size_t> indices);
    
    void updateWinner();
};

template<size_t Depth, size_t Size>
class Game<Depth, 1, Size> {
public:
    using stateType = typename std::conditional<Depth == 1, State, Game<Depth-1, 1, Size>>::type;
    using boardType = std::array<stateType, Size>;
};

template<size_t Depth, size_t Dimension, size_t Size>
Game<Depth, Dimension, Size>::operator State() const {
    return state;
}

template<size_t Depth, size_t Dimension, size_t Size>
Game<Depth, Dimension, Size>::Game() {
    initialize(board);
}

template <typename T>
concept Iterable = requires(T t) {
    t.begin();
    t.end();
};

template<size_t Depth, size_t Dimension, size_t Size>
template<typename T>
void Game<Depth, Dimension, Size>::initialize(T &container) {
    if constexpr (Iterable<T>) {
        for (auto &sub : container)
            initialize(sub);
    } else if constexpr (std::is_same_v<T, State>) {
        container = State::N;
    } else {
        container.state = State::N;
    }
}

template<size_t Depth, size_t Dimension, size_t Size>
auto& Game<Depth, Dimension, Size>::operator[](size_t i) {
    return board[i];
}

template<size_t Depth, size_t Dimension, size_t Size>
void Game<Depth, Dimension, Size>::updateWinner() {
    
}

template<size_t Depth, size_t Dimension, size_t Size>
auto& Game<Depth, Dimension, Size>::element(std::initializer_list<size_t> indices) {
    constexpr size_t expected = Depth * Dimension;
    if (indices.size() != expected) {
        throw std::invalid_argument(
            std::format("Expected {} indices, got {}", expected, indices.size())
        );
    }
    std::array<size_t, expected> idx;
    std::copy(indices.begin(), indices.end(), idx.begin());
    return element_imp(idx, std::make_index_sequence<expected>{});
}

template<size_t Depth, size_t Dimension, size_t Size>
template<std::size_t N, std::size_t... I>
auto& Game<Depth, Dimension, Size>::element_imp(const std::array<size_t, N>& idx, std::index_sequence<I...>) {
    return getElement(board, idx[I]...);
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
