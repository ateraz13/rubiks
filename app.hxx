#ifndef APP_HXX
#define APP_HXX
#include "game.hxx"
#include "window.hxx"

class App {
public:
  App();
  ~App();

  static App& instance();

  App(const App &other) = delete;
  App(App &&other) = delete;

  void init();

  Game &game();
  const Game &game() const;
  WindowSystem &win_sys();
  const WindowSystem &win_sys() const;

private:
  WindowSystem m_win_sys;
  Game m_game;
};

#endif // APP_HXX
