#ifndef GEOM_HXX
#define GEOM_HXX
#include <array>
#include <glm/vec3.hpp>
#include <optional>

namespace geom {

extern const std::array<glm::vec3, 4> square_vertices;
extern const std::array<uint16_t, 6> square_indices;
extern const std::array<glm::vec3, 4> square_normals;
extern const std::array<glm::vec3, 3> triangle_vertices;
extern const std::array<uint16_t, 3> triangle_indices;
extern const std::array<glm::vec3, 3> triangle_normals;
extern const std::array<glm::vec3, 8> cube_vertices;
extern const std::array<uint16_t, 36> cube_indices;
extern const std::array<glm::vec3, 6> cube_normals;

struct Ray {
  glm::vec3 origin;
  glm::vec3 direction;
};

struct Cube {
  glm::vec3 center;
  float side_len;
};

std::optional<glm::vec3> intersect(Ray ray, Cube cube);
} // namespace geom

#endif // GEOM_HXX
