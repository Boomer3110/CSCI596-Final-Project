#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

namespace sph {
using Vec = std::array<float, 2>;
struct Particle { Vec position{}, velocity{}; };
enum class Search { all_pairs, grid };
constexpr float dt = 0.001f;
constexpr Vec bounds{1.6f, 1.0f};

struct Scene {
    float spacing;
    std::vector<Particle> particles;
};

// Refine the same water column while keeping the kernel radius at two spacings.
inline Scene dam_break(int refinement = 1) {
    Scene scene{0.04f / refinement, {}};
    for (int y = 1; y <= 12 * refinement; ++y)
        for (int x = 1; x <= 8 * refinement; ++x)
            scene.particles.push_back({{x * scene.spacing, y * scene.spacing}, {}});
    return scene;
}

class Neighbors {
    const Scene& scene;
    const float h;
    int columns, rows;
    std::vector<std::vector<int>> cells;

    int cell(float coordinate, int count) const {
        return std::clamp(static_cast<int>(coordinate / h), 0, count - 1);
    }
public:
    Neighbors(const Scene& s, Search search)
        : scene(s), h(2 * s.spacing), columns(std::ceil(bounds[0] / h)),
          rows(std::ceil(bounds[1] / h)) {
        if (search == Search::all_pairs) return;
        cells.resize(columns * rows);
        for (int j = 0; j < static_cast<int>(s.particles.size()); ++j) {
            auto p = s.particles[j].position;
            cells[cell(p[1], rows) * columns + cell(p[0], columns)].push_back(j);
        }
    }

    template<class Visit> void each(int i, Visit visit) const {
        auto p = scene.particles[i].position;
        auto check = [&](int j) {
            auto b = scene.particles[j].position;
            Vec d{p[0] - b[0], p[1] - b[1]};
            float r2 = d[0]*d[0] + d[1]*d[1];
            if (r2 < h*h) visit(j, d, r2);
        };
        if (cells.empty()) {
            for (int j = 0; j < static_cast<int>(scene.particles.size()); ++j) check(j);
            return;
        }
        int cx = cell(p[0], columns), cy = cell(p[1], rows);
        for (int y = std::max(0, cy-1); y <= std::min(rows-1, cy+1); ++y)
            for (int x = std::max(0, cx-1); x <= std::min(columns-1, cx+1); ++x)
                for (int j : cells[y*columns+x]) check(j);
    }
};

// Returns densities at the beginning of the step. Both passes read the old state.
inline std::vector<float> step(Scene& scene, Search search) {
    const auto& particles = scene.particles;
    const int n = static_cast<int>(particles.size());
    const float h = 2 * scene.spacing, h2 = h*h;
    const float mass = 1000 * scene.spacing * scene.spacing;
    constexpr float pi = 3.14159265358979323846f;
    Neighbors neighbors(scene, search);
    std::vector<float> rho(n), pressure(n);
    for (int i = 0; i < n; ++i) {
        neighbors.each(i, [&](int, Vec, float r2) {
            float q = 1 - r2/h2;
            rho[i] += mass * (4/(pi*h2)) * q*q*q;
        });
        pressure[i] = 40 * std::max(0.f, rho[i]-1000);
    }
    auto next = particles;
    for (int i = 0; i < n; ++i) {
        Vec acceleration{0, -9.81f};
        neighbors.each(i, [&](int j, Vec d, float r2) {
            if (r2 == 0) return;
            float r = std::sqrt(r2), q = 1-r/h;
            float gradient = (-30/(pi*h*h2)) * q*q/r;
            float pair = -mass * (pressure[i]/(rho[i]*rho[i]) + pressure[j]/(rho[j]*rho[j]));
            float diffusion = -0.04f * mass * r2 * gradient / (rho[j]*(r2+0.01f*h2));
            for (int k = 0; k < 2; ++k)
                acceleration[k] += pair*gradient*d[k] +
                    diffusion*(particles[j].velocity[k]-particles[i].velocity[k]);
        });
        for (int k = 0; k < 2; ++k) {
            auto& v = next[i].velocity[k];
            auto& x = next[i].position[k];
            v += dt * acceleration[k];
            x += dt * v;
            if (x < 0 || x > bounds[k]) {
                x = std::clamp(x, 0.f, bounds[k]);
                v *= -0.4f;
            }
        }
    }
    scene.particles.swap(next);
    return rho;
}
}
