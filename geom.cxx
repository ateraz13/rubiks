#include "geom.hxx"
#include <optional>

namespace geom {

const std::array<glm::vec3, 4> square_vertices {{
    {0.5f, 0.5f, 0.0f},   // top right
    {0.5f, -0.5f, 0.0f},  // bottom right
    {-0.5f, -0.5f, 0.0f}, // bottom left
    {-0.5f, 0.5f, 0.0f}   // top left
}};
const std::array<uint16_t, 6> square_indices = {{
    0, 3, 2, // first Triangle
    2, 1, 0  // second Triangle
}};

const std::array<glm::vec3, 4> square_normals = {
    {{0.0, 0.0, 1.0f}, {0.0, 0.0, 1.0f}, {0.0, 0.0, 1.0f}, {0.0, 0.0, 1.0f}}};

const std::array<glm::vec3, 3> triangle_vertices = {{
{0.0f, 0.5f, 0.0f},
{-0.5f, -0.5f, 0.0f},
{0.5f, -0.5f, 0.0f}
}};

const std::array<uint16_t, 3> triangle_indices = {{0, 1, 2}};

const std::array<glm::vec3, 3> triangle_normals = {
    {{0.0, 0.0, 1.0f}, {0.0, 0.0, 1.0f}, {0.0, 0.0, 1.0f}}};

const std::array<glm::vec3, 8> cube_vertices = {{{-0.5f, -0.5f, -0.5f},
                                                 {0.5f, -0.5f, -0.5f},
                                                 {0.5f, 0.5f, -0.5f},
                                                 {-0.5f, 0.5f, -0.5f},
                                                 {-0.5f, -0.5f, 0.5f},
                                                 {0.5f, -0.5f, 0.5f},
                                                 {0.5f, 0.5f, 0.5f},
                                                 {-0.5f, 0.5f, 0.0f}}};

const std::array<uint16_t, 36> cube_indices = {{0, 1, 2, // Front face
                                                0, 2, 3,

                                                2, 3, 4, // Right face
                                                2, 4, 5,

                                                1, 5, 6, // Top face
                                                1, 6, 2,

                                                0, 7, 4, // Left face
                                                0, 4, 3,

                                                5, 4, 7, // Back face
                                                5, 7, 6,

                                                0, 6, 7, // Bottom face
                                                0, 1, 6}};

const std::array<glm::vec3, 6> cube_normals = {{
    glm::vec3(0, 0, -1), // Front face
    glm::vec3(0, -1, 0), // Bottom face
    glm::vec3(0, 0, 1),  // Back face
    glm::vec3(-1, 0, 0), // Left face
    glm::vec3(0, 1, 0),  // Top face
    glm::vec3(1, 0, 0)   // Right face
}};

}
