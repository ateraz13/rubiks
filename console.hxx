#ifndef CONSOLE_HXX
#define CONSOLE_HXX
#include <string>
#include <vector>
#include <array>
#include <sstream>


extern size_t m_console_count;
// FIXME: May need to be thread safe in the future.
class Console {
public:
    Console();
    Console(const Console& other);
    Console(Console&& other);
    ~Console();

    void draw();

    bool visible() const;

    void toggle_visible();
    void hide();
    void show();
private:

    static const size_t m_prompt_capacity = 1024;
    std::array<char, m_prompt_capacity> m_prompt;
    std::vector<std::string> m_history;
    std::stringstream m_output;
    bool m_is_visible = false;
    std::string m_title = "Console";
    int width = 800, height = 600;
};


#endif // CONSOLE_HXX
