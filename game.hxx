#ifndef GAME_HXX
#define GAME_HXX
#include "geom.hxx"
#include "gl.hxx"
#include "gfx.hxx"
#include "utility.hxx"
#include <GLFW/glfw3.h>
#include <array>
#include <chrono>
#include <cstdint>
// #include <glad/gl.h>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include "const.hxx"

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

// FIXME: This Game class should be thread safe, it is not at the moment.
class Game {
public:
  ~Game();

  static Game &instance();

  void start();
  void update();
  void stop();
  void acknowledge_main_window_resize(int new_width, int new_height);

  void save(const std::string &name);
  double get_current_time() const;

  void operator=(const Game &other) = delete;
  Game(const Game &other) = delete;

  using KeyboardKey = int;

  enum struct KeyState { PRESSED, RELEASED };

  struct KeyEvent {
    std::weak_ptr<GLFWwindow> window;
    KeyboardKey key;
    KeyState state;

    bool operator<(const KeyEvent &other) const;
  };

  double current_time() const;

private:
  enum Action {
    QUIT_GAME,
    ROTATE_1ST_COLUMN_FORWARD,
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
    ACTION_COUNT // Always last
  };

  static Game &init();
  Game();

  void update_current_time();

  void init_window_system();
  void init_input_system();
  static void handle_inputs(GLFWwindow *win, Game::KeyboardKey key,
                            int scancode, int action, int mods);

  RubiksCube m_rcube;
  uint64_t m_last_time = 0;
  double m_current_time = 0.0;
  uint64_t delta_time = 0;
  bool m_is_running = false;
  float m_animation_speed = 1.0f;
  std::shared_ptr<GLFWwindow> m_main_window;
  std::queue<Action> m_action_queue;
  std::map<KeyEvent, Action> m_keymap;
  gfx::Graphics m_gfx;
  std::chrono::time_point<std::chrono::steady_clock> m_last_frame_timepoint;
};


#endif // GAME_HXX
