//
//  Lines.hpp
//  Multi Dimensional XOX
//
//  Created by Erhan Türker on 2.02.2025.
//
#pragma once
#include <array>

using std::size_t;

constexpr size_t pow_int(size_t base, size_t exp) {
    return exp == 0 ? 1 : base * pow_int(base, exp - 1);
}

template <size_t DIM, size_t N>
struct CubeLines {
    using Point = std::array<int, DIM>;
    using Line  = std::array<Point, N>;

    static constexpr size_t max_possible_lines = pow_int(3, DIM) * pow_int(N, DIM);

    struct LinesResult {
        std::array<Line, max_possible_lines> lines{};
        size_t count = 0;
    };

    static constexpr bool in_bounds(const Point& p) {
        for (size_t i = 0; i < DIM; ++i)
            if (p[i] < 0 || p[i] >= int(N))
                return false;
        return true;
    }

    static constexpr Point add(const Point& a, const Point& b) {
        Point result{};
        for (size_t i = 0; i < DIM; ++i)
            result[i] = a[i] + b[i];
        return result;
    }

    static constexpr bool generate_line(const Point& start, const Point& d, Line& line) {
        Point p = start;
        for (size_t i = 0; i < N; ++i) {
            if (!in_bounds(p))
                return false;
            line[i] = p;
            p = add(p, d);
        }
        return true;
    }

    static constexpr bool is_canonical(const Point& d) {
        for (size_t i = 0; i < DIM; ++i) {
            if (d[i] != 0)
                return d[i] > 0;
        }
        return false;
    }

    static constexpr LinesResult generate_all_lines() {
        LinesResult result{};
        Point start{};

        auto rec_start = [&] (auto& self, int dim) -> void {
            if (dim == int(DIM)) {
                Point d{};
                auto rec_dir = [&] (auto& self2, int d_index) -> void {
                    if (d_index == int(DIM)) {
                        bool isZero = true;
                        for (size_t k = 0; k < DIM; ++k) {
                            if (d[k] != 0) { isZero = false; break; }
                        }
                        if (!isZero && is_canonical(d)) {
                            Line line{};
                            if (generate_line(start, d, line)) {
                                result.lines[result.count] = line;
                                result.count++;
                            }
                        }
                        return;
                    }
                    for (int v = -1; v <= 1; ++v) {
                        d[d_index] = v;
                        self2(self2, d_index + 1);
                    }
                };
                rec_dir(rec_dir, 0);
                return;
            }
            for (int i = 0; i < int(N); ++i) {
                start[dim] = i;
                self(self, dim + 1);
            }
        };

        rec_start(rec_start, 0);
        return result;
    }
};

