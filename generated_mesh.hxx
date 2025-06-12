#ifndef MESH
#define MESH
#include <array>
struct SimpleMesh {
  enum struct BufferType {
    COLOR, 
    POSITION, 
    INDEX,
    COUNT
  };
  enum struct AttribType {
    COLOR, 
    POSITION, 
    COUNT
  };
  void init();
  void draw();
  void send_color_data(/*args*/);
  void send_position_data(/*args*/);
private: 
  const std::array<const char*, static_cast<std::size_t>(AttribType::COUNT)> m_attrib_names = {
    "color", 
    "position", 
  };
  const std::array<GLuint, static_cast<std::size_t>(BufferType::COUNT)> m_buffers = {
    "color", 
    "position", 
  };
};

#endif //MESH
