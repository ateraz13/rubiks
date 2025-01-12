#include "keys.hxx"
#include <cstdint>
#include <map>

const char *keycode_str(KeyCode keycode) {
  static std::map<KeyCode, const char *> keycode_strings = {
      {RUBIKS_KEY_SPACE, "KEY_SPACE"},
      {RUBIKS_KEY_APOSTROPHE, "KEY_APOSTROPHE"},
      {RUBIKS_KEY_COMMA, "KEY_COMMA"},
      {RUBIKS_KEY_MINUS, "KEY_MINUS"},
      {RUBIKS_KEY_PERIOD, "KEY_PERIOD"},
      {RUBIKS_KEY_SLASH, "KEY_SLASH"},
      {RUBIKS_KEY_0, "KEY_0"},
      {RUBIKS_KEY_1, "KEY_1"},
      {RUBIKS_KEY_2, "KEY_2"},
      {RUBIKS_KEY_3, "KEY_3"},
      {RUBIKS_KEY_4, "KEY_4"},
      {RUBIKS_KEY_5, "KEY_5"},
      {RUBIKS_KEY_6, "KEY_6"},
      {RUBIKS_KEY_7, "KEY_7"},
      {RUBIKS_KEY_8, "KEY_8"},
      {RUBIKS_KEY_9, "KEY_9"},
      {RUBIKS_KEY_SEMICOLON, "KEY_SEMICOLON"},
      {RUBIKS_KEY_EQUAL, "KEY_EQUAL"},
      {RUBIKS_KEY_A, "KEY_A"},
      {RUBIKS_KEY_B, "KEY_B"},
      {RUBIKS_KEY_C, "KEY_C"},
      {RUBIKS_KEY_D, "KEY_D"},
      {RUBIKS_KEY_E, "KEY_E"},
      {RUBIKS_KEY_F, "KEY_F"},
      {RUBIKS_KEY_G, "KEY_G"},
      {RUBIKS_KEY_H, "KEY_H"},
      {RUBIKS_KEY_I, "KEY_I"},
      {RUBIKS_KEY_J, "KEY_J"},
      {RUBIKS_KEY_K, "KEY_K"},
      {RUBIKS_KEY_L, "KEY_L"},
      {RUBIKS_KEY_M, "KEY_M"},
      {RUBIKS_KEY_N, "KEY_N"},
      {RUBIKS_KEY_O, "KEY_O"},
      {RUBIKS_KEY_P, "KEY_P"},
      {RUBIKS_KEY_Q, "KEY_Q"},
      {RUBIKS_KEY_R, "KEY_R"},
      {RUBIKS_KEY_S, "KEY_S"},
      {RUBIKS_KEY_T, "KEY_T"},
      {RUBIKS_KEY_U, "KEY_U"},
      {RUBIKS_KEY_V, "KEY_V"},
      {RUBIKS_KEY_W, "KEY_W"},
      {RUBIKS_KEY_X, "KEY_X"},
      {RUBIKS_KEY_Y, "KEY_Y"},
      {RUBIKS_KEY_Z, "KEY_Z"},
      {RUBIKS_KEY_LEFT_BRACKET, "KEY_LEFT_BRACKET"},
      {RUBIKS_KEY_BACKSLASH, "KEY_BACKSLASH"},
      {RUBIKS_KEY_RIGHT_BRACKET, "KEY_RIGHT_BRACKET"},
      {RUBIKS_KEY_GRAVE_ACCENT, "KEY_GRAVE_ACCENT"},
      {RUBIKS_KEY_WORLD_1, "KEY_WORLD_1"},
      {RUBIKS_KEY_WORLD_2, "KEY_WORLD_2"},
      {RUBIKS_KEY_ESCAPE, "KEY_ESCAPE"},
      {RUBIKS_KEY_ENTER, "KEY_ENTER"},
      {RUBIKS_KEY_TAB, "KEY_TAB"},
      {RUBIKS_KEY_BACKSPACE, "KEY_BACKSPACE"},
      {RUBIKS_KEY_INSERT, "KEY_INSERT"},
      {RUBIKS_KEY_DELETE, "KEY_DELETE"},
      {RUBIKS_KEY_RIGHT, "KEY_RIGHT"},
      {RUBIKS_KEY_LEFT, "KEY_LEFT"},
      {RUBIKS_KEY_DOWN, "KEY_DOWN"},
      {RUBIKS_KEY_UP, "KEY_UP"},
      {RUBIKS_KEY_PAGE_UP, "KEY_PAGE_UP"},
      {RUBIKS_KEY_PAGE_DOWN, "KEY_PAGE_DOWN"},
      {RUBIKS_KEY_HOME, "KEY_HOME"},
      {RUBIKS_KEY_END, "KEY_END"},
      {RUBIKS_KEY_CAPS_LOCK, "KEY_CAPS_LOCK"},
      {RUBIKS_KEY_SCROLL_LOCK, "KEY_SCROLL_LOCK"},
      {RUBIKS_KEY_NUM_LOCK, "KEY_NUM_LOCK"},
      {RUBIKS_KEY_PRINT_SCREEN, "KEY_PRINT_SCREEN"},
      {RUBIKS_KEY_PAUSE, "KEY_PAUSE"},
      {RUBIKS_KEY_F1, "KEY_F1"},
      {RUBIKS_KEY_F2, "KEY_F2"},
      {RUBIKS_KEY_F3, "KEY_F3"},
      {RUBIKS_KEY_F4, "KEY_F4"},
      {RUBIKS_KEY_F5, "KEY_F5"},
      {RUBIKS_KEY_F6, "KEY_F6"},
      {RUBIKS_KEY_F7, "KEY_F7"},
      {RUBIKS_KEY_F8, "KEY_F8"},
      {RUBIKS_KEY_F9, "KEY_F9"},
      {RUBIKS_KEY_F10, "KEY_F10"},
      {RUBIKS_KEY_F11, "KEY_F11"},
      {RUBIKS_KEY_F12, "KEY_F12"},
      {RUBIKS_KEY_F13, "KEY_F13"},
      {RUBIKS_KEY_F14, "KEY_F14"},
      {RUBIKS_KEY_F15, "KEY_F15"},
      {RUBIKS_KEY_F16, "KEY_F16"},
      {RUBIKS_KEY_F17, "KEY_F17"},
      {RUBIKS_KEY_F18, "KEY_F18"},
      {RUBIKS_KEY_F19, "KEY_F19"},
      {RUBIKS_KEY_F20, "KEY_F20"},
      {RUBIKS_KEY_F21, "KEY_F21"},
      {RUBIKS_KEY_F22, "KEY_F22"},
      {RUBIKS_KEY_F23, "KEY_F23"},
      {RUBIKS_KEY_F24, "KEY_F24"},
      {RUBIKS_KEY_F25, "KEY_F25"},
      {RUBIKS_KEY_KP_0, "KEY_KP_0"},
      {RUBIKS_KEY_KP_1, "KEY_KP_1"},
      {RUBIKS_KEY_KP_2, "KEY_KP_2"},
      {RUBIKS_KEY_KP_3, "KEY_KP_3"},
      {RUBIKS_KEY_KP_4, "KEY_KP_4"},
      {RUBIKS_KEY_KP_5, "KEY_KP_5"},
      {RUBIKS_KEY_KP_6, "KEY_KP_6"},
      {RUBIKS_KEY_KP_7, "KEY_KP_7"},
      {RUBIKS_KEY_KP_8, "KEY_KP_8"},
      {RUBIKS_KEY_KP_9, "KEY_KP_9"},
      {RUBIKS_KEY_KP_DECIMAL, "KEY_KP_DECIMAL"},
      {RUBIKS_KEY_KP_DIVIDE, "KEY_KP_DIVIDE"},
      {RUBIKS_KEY_KP_MULTIPLY, "KEY_KP_MULTIPLY"},
      {RUBIKS_KEY_KP_SUBTRACT, "KEY_KP_SUBTRACT"},
      {RUBIKS_KEY_KP_ADD, "KEY_KP_ADD"},
      {RUBIKS_KEY_KP_ENTER, "KEY_KP_ENTER"},
      {RUBIKS_KEY_KP_EQUAL, "KEY_KP_EQUAL"},
      {RUBIKS_KEY_LEFT_SHIFT, "KEY_LEFT_SHIFT"},
      {RUBIKS_KEY_LEFT_CONTROL, "KEY_LEFT_CONTROL"},
      {RUBIKS_KEY_LEFT_ALT, "KEY_LEFT_ALT"},
      {RUBIKS_KEY_LEFT_SUPER, "KEY_LEFT_SUPER"},
      {RUBIKS_KEY_RIGHT_SHIFT, "KEY_RIGHT_SHIFT"},
      {RUBIKS_KEY_RIGHT_CONTROL, "KEY_RIGHT_CONTROL"},
      {RUBIKS_KEY_RIGHT_ALT, "KEY_RIGHT_ALT"},
      {RUBIKS_KEY_RIGHT_SUPER, "KEY_RIGHT_SUPER"},
      {RUBIKS_KEY_MENU, "KEY_MENU"},
      {RUBIKS_KEY_LAST, "KEY_LAST"}};
  if (auto it = keycode_strings.find(keycode); it != keycode_strings.end()) {
    return it->second;
  } else {
    return "Undefined key!";
  }
}

std::ostream &operator<<(std::ostream &strm, KeyCode keycode) {
  strm << keycode_str(keycode);
  return strm;
}

std::strong_ordering KeyCode::operator<=>(const KeyCode &other) {
  return value <=> other.value;
}

bool KeyCode::operator>(const KeyCode &other) { return value > other.value; }
bool KeyCode::operator<(const KeyCode &other) { return value < other.value; }
bool KeyCode::operator==(const KeyCode &other) { return value == other.value; }
bool KeyCode::operator>(KeyCode other) const { return value > other.value; }
bool KeyCode::operator<(KeyCode other) const { return value < other.value; }
bool KeyCode::operator==( KeyCode other) const { return value == other.value; }
