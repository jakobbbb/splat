#include "sorting.hpp"

#include <iostream>

namespace splat {

void Sorting::start(size_t num_gaussians,
                    std::pair<glm::vec3, glm::vec3> bounds,
                    std::vector<Gaussian> const& gaussians) {
    this->num_gaussians = num_gaussians;
    this->bounds = bounds;
    this->gaussians = gaussians;

    sorted_front = std::vector<int>(num_gaussians, 0);
    sorted_back = std::vector<int>(num_gaussians, 0);

    sort_thread = std::thread(&Sorting::loop, this);
}

void Sorting::stop() {
    do_sort = false;
    sort_thread.join();
}

void Sorting::loop() {
    while (do_sort) {
        sort_back();
        swap();
    }
}

void Sorting::swap() {
    // TODO mutex with get_sorted?
    std::swap(sorted_front, sorted_back);
    new_sort_available = true;
}

void Sorting::update(glm::vec3 const& cam_pos) {
    this->cam_pos = cam_pos;
}

std::vector<int>& Sorting::get_sorted() {
    // TODO mutex for swap?
    new_sort_available = false;
    return sorted_front;
}

size_t Sorting::get_sort_key(Gaussian const& g, glm::vec4 const& cam_pos, float max_dist) {
    auto v = -cam_pos - g.pos;
    float d = v.x * v.x + v.y * v.y + v.z * v.z;    // dot product
    float d_normalized = NUM_BUCKETS * d / max_dist;  // between 0 and n_buckets
    return glm::min(d_normalized, (float)NUM_BUCKETS - 1);
}

void Sorting::sort_back() {
    auto start_time = std::chrono::system_clock::now();

    std::vector<size_t> count(Sorting::NUM_BUCKETS + 1, 0);

    std::vector<size_t> distances{};
    distances.reserve(num_gaussians);

    float max_dist = 1.2f * glm::distance(bounds.first, bounds.second);
    max_dist *= max_dist;

    for (auto const& g : gaussians) {
        size_t d_int = Sorting::get_sort_key(g, glm::vec4(cam_pos, 1), max_dist);
        ++count[d_int];
        distances.push_back(d_int);
    }

    for (int i = 1; i < count.size(); ++i) {
        count[i] = count[i] + count[i - 1];
    }

    for (int i = num_gaussians - 1; i >= 0; --i) {
        size_t j = distances[i];
        --count[j];
        sorted_back[count[j]] = i;
    }

    ++stats.first;

    auto end_time = std::chrono::system_clock::now();
    std::chrono::duration<double> duration_in_s = end_time - start_time;
    stats.second += duration_in_s.count();
}

void Sorting::reset_stats() {
    stats.first = 0;
    stats.second = 0.0f;
}

}  // namespace splat
