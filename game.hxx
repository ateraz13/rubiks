#ifndef GAME_HXX
#define GAME_HXX
#include <GLFW/glfw3.h>
#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <memory>
#include "geom.hxx"
#include "gfx.hxx"
#include "utility.hxx"

class RubiksCube {
public:
  using CellID = int;
  // Returns negative number if not cell was intersected.
  CellID ray_intersection(geom::Ray ray) const;

  void rotate_1st_column_forward();
  void rotate_2nd_column_forward();
  void rotate_3rd_column_forward();
  void rotate_1st_column_backwards();
  void rotate_2nd_column_backwards();
  void rotate_3rd_column_backwards();
  void rotate_1st_row_forward();
  void rotate_2nd_row_forward();
  void rotate_3rd_row_forward();
  void rotate_1st_row_backwards();
  void rotate_2nd_row_backwards();
  void rotate_3rd_row_backwards();
  void reset();

private:
  std::array<CellID, 9 * 3> m_cells;
};

class Game {
public:
  Game();
  ~Game();

  void start();
  void update();
  void stop();

  void save(const std::string &name);
  void get_current_time() const;

private:
  void update_current_time();

  RubiksCube m_rcube;
  gfx::Graphics m_gfx;
  uint64_t m_last_time;
  uint64_t m_current_time;
  uint64_t delta_time;
  bool m_is_running;

  using KeyboardKey = int;

  enum Action {
    ROTATE_1ST_COLUMN_FORWARD = 0,
    ROTATE_2ND_COLUMN_FORWARD,
    ROTATE_3RD_COLUMN_FORWARD,
    ROTATE_1ST_COLUMN_BACKWARDS,
    ROTATE_2ND_COLUMN_BACKWARDS,
    ROTATE_3RD_COLUMN_BACKWARDS,
    ROTATE_1ST_ROW_FORWARD,
    ROTATE_2ND_ROW_FORWARD,
    ROTATE_3RD_ROW_FORWARD,
    ROTATE_1ST_ROW_BACKWARDS,
    ROTATE_2ND_ROW_BACKWARDS,
    ROTATE_3RD_ROW_BACKWARDS,
    ACTION_COUNT
  };

  void init_window_system();
  void init_input_system();

  std::map<KeyboardKey, Action> m_keymap;
  float m_animation_speed;
  std::shared_ptr<GLFWwindow> main_window;
};

extern const int MAIN_WINDOW_DEFAULT_HEIGHT;
extern const int MAIN_WINDOW_DEFAULT_WIDTH;
extern const char* MAIN_WINDOW_DEFAULT_TITLE;

#endif //GAME_HXX
