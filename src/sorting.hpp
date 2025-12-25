#ifndef SORTING_HPP
#define SORTING_HPP

#include <thread>
#include <vector>
#include "app.hpp"

namespace splat {


class Sorting {
   public:
    void loop();
    static size_t get_sort_key(Gaussian const& g, glm::vec4 const& cam_pos, float max_dist);
    static const size_t NUM_BUCKETS = 65535;

   private:
    void sort(std::vector<int>& indices, std::vector<Gaussian> const& gaussians);

    // double buffering
    std::vector<int> sorted_front;
    std::vector<int> sorted_back;
};
}

#endif  // SORTING_HPP
