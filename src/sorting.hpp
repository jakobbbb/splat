#ifndef SORTING_HPP
#define SORTING_HPP

#include <glm/gtc/quaternion.hpp>
#include <thread>
#include <vector>
#include "util.hpp"

namespace splat {

class Sorting {
   public:
    void start(size_t num_gaussians,
               std::pair<glm::vec3, glm::vec3> bounds,
               std::vector<Gaussian> const& gaussians);
    void stop();
    static size_t get_sort_key(Gaussian const& g, glm::vec4 const& cam_pos, float max_dist);

    void update(glm::vec3 const& cam_pos);
    std::vector<int>& get_sorted();
    bool is_new_sort_available() const { return new_sort_available; }

    std::pair<size_t, float> get_stats() const { return stats; };
    void reset_stats();

    static const size_t NUM_BUCKETS = (1 << 20) - 1;

   private:
    void sort_back();
    void loop();

    // double buffering
    std::vector<int> sorted_front;
    std::vector<int> sorted_back;
    void swap();

    std::thread sort_thread;
    bool do_sort = true;

    size_t num_gaussians;
    std::vector<Gaussian> gaussians;
    std::pair<glm::vec3, glm::vec3> bounds;
    glm::vec3 cam_pos;

    bool new_sort_available;

    // Number of sorts done, total time spent
    std::pair<size_t, float> stats;
};
}  // namespace splat

#endif  // SORTING_HPP
