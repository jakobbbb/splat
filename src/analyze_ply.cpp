#include "external/happly.h"
#include <glm/common.hpp>
#include <glm/glm.hpp>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "usage: " << argv[0] << " <point_cloud.ply>\n";
        return 1;
    }
    char* ply_path = argv[1];
    auto ply = happly::PLYData(ply_path);

    std::vector<std::vector<float>> scales{};
    scales.emplace_back(ply.getElement("vertex").getProperty<float>("scale_0"));
    scales.emplace_back(ply.getElement("vertex").getProperty<float>("scale_1"));
    scales.emplace_back(ply.getElement("vertex").getProperty<float>("scale_2"));

    std::cout << "value,kind\n";
    for (size_t i_gaussian = 0; i_gaussian < scales[0].size(); ++i_gaussian) {
        glm::vec3 scale = {
            glm::abs(glm::exp(scales[0][i_gaussian])),
            glm::abs(glm::exp(scales[1][i_gaussian])),
            glm::abs(glm::exp(scales[2][i_gaussian]))
        };
        float min = glm::min(glm::min(scale.x, scale.y), scale.z);
        float max = glm::max(glm::max(scale.x, scale.y), scale.z);

        float mid = min;
        for (int i = 0; i < 2; ++i) {
            if (min < scale[i] && scale[i] < max) {
                mid = scale[i];
            }
        }

        min /= max;
        mid /= max;
        std::cout << min << ",min\n";
        std::cout << mid << ",mid\n";
        //std::cout << max << ",max\n";
    }
}
