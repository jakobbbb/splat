#include "sorting.hpp"

#include <iostream>

namespace splat {

void Sorting::start() {
    sort_thread = std::thread(&Sorting::loop, this);
}

void Sorting::stop() {
    do_sort = false;
    sort_thread.join();
}

void Sorting::loop() {
    while (do_sort) {
        // TODO
    }
}

size_t Sorting::get_sort_key(Gaussian const& g, glm::vec4 const& cam_pos, float max_dist) {
    auto v = -cam_pos - g.pos;
    float d = v.x * v.x + v.y * v.y + v.z * v.z;    // dot product
    float d_normalized = NUM_BUCKETS * d / max_dist;  // between 0 and n_buckets
    return glm::min(d_normalized, (float)NUM_BUCKETS - 1);
}

void Sorting::sort(std::vector<int>& indices, std::vector<Gaussian> const& gaussians) {
    // TODO
}

}  // namespace splat
