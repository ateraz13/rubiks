#ifndef GAME_HXX
#define GAME_HXX
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <memory>
#include <queue>
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
  ~Game();

  static Game& instance() {
    static Game game;
    return game;
  }

  void start();
  void update();
  void stop();

  void save(const std::string &name);
  double get_current_time() const;

  // We don't want assignment or copying to happen.
  void operator=(const Game& other) = delete;
  Game(const Game& other) = delete;

  using KeyboardKey = int;

  enum struct KeyState {
    PRESSED, RELEASED
  };

  struct KeyEvent {
    std::weak_ptr<GLFWwindow> window;
    KeyboardKey key;
    KeyState state;

    bool operator<(const KeyEvent& other) const;
  };
private:
  Game();

  void update_current_time();

  RubiksCube m_rcube;
  gfx::Graphics m_gfx;
  uint64_t m_last_time;
  double m_current_time;
  uint64_t delta_time;
  bool m_is_running;

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

  void init_window_system();
  void init_input_system();
  static void handle_inputs(GLFWwindow *win, Game::KeyboardKey key,
                            int scancode, int action, int mods);

  float m_animation_speed;
  std::shared_ptr<GLFWwindow> m_main_window;
  std::queue<Action> action_queue;
  std::map<KeyEvent, Action> m_keymap;
};


extern const int MAIN_WINDOW_DEFAULT_HEIGHT;
extern const int MAIN_WINDOW_DEFAULT_WIDTH;
extern const char* MAIN_WINDOW_DEFAULT_TITLE;

#endif //GAME_HXX
