#include "sph.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace sph;

void verify() {
    auto a = dam_break();
    a.particles.push_back({{0.079f, 0.079f}, {}});
    a.particles.push_back({{0.081f, 0.081f}, {}});
    auto b = a;
    for (int t = 0; t < 20; ++t) {
        Neighbors reference(a, Search::all_pairs), grid(a, Search::grid);
        for (int i = 0; i < static_cast<int>(a.particles.size()); ++i) {
            std::vector<int> expected, actual;
            reference.each(i, [&](int j, Vec, float){ expected.push_back(j); });
            grid.each(i, [&](int j, Vec, float){ actual.push_back(j); });
            std::sort(actual.begin(), actual.end());
            if (actual != expected) throw std::runtime_error("Neighbor mismatch");
        }
        auto da = step(a, Search::all_pairs), db = step(b, Search::grid);
        auto close = [](float x, float y) {
            if (!(std::abs(x-y) <= 0.0001f * std::max(1.f, std::abs(x))))
                throw std::runtime_error("Solver mismatch");
        };
        for (size_t i = 0; i < da.size(); ++i) {
            close(da[i], db[i]);
            for (int k = 0; k < 2; ++k) {
                close(a.particles[i].position[k], b.particles[i].position[k]);
                close(a.particles[i].velocity[k], b.particles[i].velocity[k]);
            }
        }
    }
    Scene drop{0.04f, {{{0.5f, 0.000001f}, {}}}};
    step(drop, Search::grid);
    if (drop.particles[0].position[1] != 0 || drop.particles[0].velocity[1] <= 0)
        throw std::runtime_error("Floor bounce failed");
    std::cout << "Neighbor sets, CPU trajectories and floor bounce passed.\n";
}

double milliseconds(int refinement, Search search) {
    std::array<double, 5> trials;
    for (auto& time : trials) {
        auto scene = dam_break(refinement);
        for (int i = 0; i < 3; ++i) step(scene, search);
        auto start = std::chrono::steady_clock::now();
        for (int i = 0; i < 10; ++i) step(scene, search);
        time = std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count()/10;
    }
    std::sort(trials.begin(), trials.end());
    return trials[2];
}

int main(int argc, char** argv) {
    std::string mode = argc == 2 ? argv[1] : "";
    if (mode == "check") verify();
    else if (mode == "benchmark") {
        std::cout << "particles,all_pairs_ms,grid_ms,speedup\n";
        for (int scale : {1,2,3,4,6}) {
            double baseline = milliseconds(scale, Search::all_pairs);
            double grid = milliseconds(scale, Search::grid);
            std::cout << 96*scale*scale << ',' << baseline << ',' << grid << ',' << baseline/grid << '\n';
        }
    } else if (mode == "reference") {
        auto scene = dam_break();
        auto initial = scene.particles;
        auto densities = step(scene, Search::all_pairs);
        std::cout << "id,x0,y0,density0,x1,y1,vx1,vy1\n" << std::setprecision(9);
        for (size_t i = 0; i < initial.size(); ++i) {
            auto p = scene.particles[i];
            std::cout << i << ',' << initial[i].position[0] << ',' << initial[i].position[1]
                      << ',' << densities[i] << ',' << p.position[0] << ',' << p.position[1]
                      << ',' << p.velocity[0] << ',' << p.velocity[1] << '\n';
        }
    } else {
        std::cerr << "Usage: sph check | reference | benchmark\n";
        return 1;
    }
}
