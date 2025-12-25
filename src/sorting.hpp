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
    const bool is_new_sort_available() { return new_sort_available; };

    static const size_t NUM_BUCKETS = 65535;

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
};
}  // namespace splat

#endif  // SORTING_HPP
