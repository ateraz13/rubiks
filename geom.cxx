#include "geom.hxx"
#include <optional>

namespace geom {

const std::array<glm::vec3, 4> square_vertices{{
    {0.5f, 0.5f, -1.0f},   // top right
    {0.5f, -0.5f, -1.0f},  // bottom right
    {-0.5f, -0.5f, -1.0f}, // bottom left
    {-0.5f, 0.5f, -1.0f}   // top left
}};

const std::array<uint16_t, 6> square_indices = {{
    0, 3, 2, // first Triangle
    2, 1, 0  // second Triangle
}};

const std::array<glm::vec3, 4> square_normals = {
    {{0.0, 0.0, 1.0f}, {0.0, 0.0, 1.0f}, {0.0, 0.0, 1.0f}, {0.0, 0.0, 1.0f}}};

const std::array<glm::vec3, 3> triangle_vertices = {
    {{0.0f, 0.5f, 0.0f}, {-0.5f, -0.5f, 0.0f}, {0.5f, -0.5f, 0.0f}}};

const std::array<uint16_t, 3> triangle_indices = {{0, 1, 2}};

const std::array<glm::vec3, 3> triangle_normals = {
    {{0.0, 0.0, 1.0f}, {0.0, 0.0, 1.0f}, {0.0, 0.0, 1.0f}}};

const std::array<glm::vec3, 8> cube_vertices = {{
    {-0.5f, -0.5f, -0.5f}, // Front Top Left
    {-0.5f, 0.5f, -0.5f}, // Front Bottom Left
    {0.5f, -0.5f, -0.5f}, // Front Top Right
    {0.5f, 0.5f, -0.5f}, // Front Botton Right
    {-0.5f, -0.5f, 0.5f}, // Back Top Left
    {-0.5f, 0.5f, 0.5f}, // Back Bottom Left
    {0.5f, -0.5f, 0.5f}, // Back Top Right
    {0.5f, 0.5f, 0.5f}, // Back Botton Right
}};

const std::array<uint16_t, 36> cube_indices = {{
    0, 1, 2, // Front Face
    1, 3, 2,
    2, 3, 6, // Right Face
    6, 3, 7,
    7, 5, 6, // Back Face
    6, 5, 4,
    4, 5, 0, // Left Face
    0, 5, 1,
    1, 3, 7, // Bottom Face
    7, 5, 1,
    6, 0, 4, // Top Face
    0, 6, 2
}};

const std::array<glm::vec3, 6> cube_normals = {{
    glm::vec3(0, 0, -1), // Front face
    glm::vec3(1, 0, 0),   // Right face
    glm::vec3(0, 0, 1),  // Back face
    glm::vec3(-1, 0, 0), // Left face
    glm::vec3(0, -1, 0), // Bottom face
    glm::vec3(0, 1, 0)  // Top face
}};

} // namespace geom
