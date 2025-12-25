#ifndef SORTING_HPP
#define SORTING_HPP

#include <thread>
#include <vector>
#include "util.hpp"

namespace splat {

class Sorting {
   public:
    void start();
    void stop();
    static size_t get_sort_key(Gaussian const& g, glm::vec4 const& cam_pos, float max_dist);

    static const size_t NUM_BUCKETS = 65535;

   private:
    void sort(std::vector<int>& indices, std::vector<Gaussian> const& gaussians);
    void loop();

    // double buffering
    std::vector<int> sorted_front;
    std::vector<int> sorted_back;

    std::thread sort_thread;
    bool do_sort = true;
};
}  // namespace splat

#endif  // SORTING_HPP
