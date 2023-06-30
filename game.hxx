#ifndef GAME_HXX
#define GAME_HXX
#include "gfx.hxx"
#include <array>
#include <cstdint>

class RubiksCube {
  using CellID = int;
  // Returns negative number if not cell was intersected.
  CellID ray_intersection(Ray ray) const;

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

class GameSettings {

    GameSettings();
    GameSettings(const GameSettings& other);

    void get_graphical_settings() const;
    void set_graphical_settings() const;
};

class Game {

  explicit Game(GameSettings settings);
  ~Game();

  void start();
  void update();
  void stop();

  void save(const std::string &name);
  void get_current_time() const;

private:
  RubiksCube m_rcube;
  gfx::Graphics m_gfx;
  uint64_t m_last_time;
  uint64_t m_current_time;
  uint64_t delta_time;
  bool m_is_running;
};

#endif // GAME_HXX
