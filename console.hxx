#ifndef CONSOLE_HXX
#define CONSOLE_HXX
#include "action.hxx"
#include <array>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

extern size_t m_console_count;
// FIXME: May need to be thread safe in the future.
// FIXME: Commands may be running asynchrounously, especially if they cannot
// produce output within a single frame.
class Console : public std::streambuf {
public:
  Console();
  Console(const Console &other);
  Console(Console &&other);
  ~Console();

  void draw();
  bool visible() const;
  void toggle_visible();
  void hide();
  void show();

protected:
  virtual std::streamsize xsputn(const char_type *s, std::streamsize n) override;

private:
  static const size_t m_prompt_capacity = 1024;
  std::array<char, m_prompt_capacity> m_prompt;
  std::vector<std::string> m_history;
  std::stringstream m_output;
  bool m_is_visible = false;
  std::string m_title = "Console";
  int m_width = 800, m_height = 600;
  std::map<std::string, std::unique_ptr<Action>> m_command_list;
  float m_last_scroll = 0.0f;
  float m_last_max_scroll = 0.0f;
  std::mutex m_lock;
};

#endif // CONSOLE_HXX
