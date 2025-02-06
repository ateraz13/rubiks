#include "console.hxx"
#include <imgui.h>
#include <iostream>
#include <utility>

size_t m_console_count = 0;

Console::Console() {
  m_console_count += 1;
  std::stringstream strm;
  strm << "Console<" << m_console_count << ">";
  m_output << "Sauce!" << std::endl;
  m_output << "Sauce!" << std::endl;
  m_output << "Sauce!" << std::endl;
  m_output << "Sauce!" << std::endl;
  m_title = strm.str();
  m_command_list["quit"] = std::make_unique<ActionCommand>(std::make_shared<QuitAction>());
  m_command_list["print"] = std::make_unique<PrintCommand>();
}

Console::~Console() { m_console_count -= 1; }

Console::Console(Console &&other)
    : m_history(std::move(other.m_history)),
      m_output(std::move(other.m_output)), m_prompt(std::move(other.m_prompt)),
      m_is_visible(std::move(other.m_is_visible)), m_width(other.m_width),
      m_height(other.m_height) {}

Console::Console(const Console &other)
    : m_history(other.m_history), m_output(), m_prompt(other.m_prompt),
      m_is_visible(other.m_is_visible), m_width(other.m_width),
      m_height(other.m_height) {

  m_output << other.m_output.rdbuf();
  m_console_count += 1;
}

bool Console::visible() const { return m_is_visible; }

void Console::toggle_visible() { m_is_visible = !m_is_visible; }

void Console::hide() { m_is_visible = false; }

void Console::show() { m_is_visible = true; }

void Console::draw() {
  ImGui::SetNextWindowSize(ImVec2(m_width, m_height), ImGuiCond_FirstUseEver);
  if (!ImGui::Begin(m_title.c_str(), &m_is_visible)) {
    ImGui::End();
    return;
  }

  if (ImGui::BeginPopupContextItem()) {
    if (ImGui::MenuItem("Close Console")) {
      m_is_visible = false;
    }
    ImGui::EndPopup();
  }

  ImGui::Separator();

  if (ImGui::BeginPopup("Options")) {
    ImGui::EndPopup();
  }

  if (ImGui::Button("Options")) {
    ImGui::OpenPopup("Options");
  }

  ImGui::Separator();

  int footer_height = 20;
  if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height),
                        ImGuiChildFlags_NavFlattened,
                        ImGuiWindowFlags_HorizontalScrollbar)) {
    if (ImGui::BeginPopupContextWindow()) {
      // Add some popup items.
      ImGui::EndPopup();
    }

    ImGui::TextUnformatted(m_output.str().c_str());

    if(m_last_scroll == m_last_max_scroll) {
      ImGui::SetScrollHereY(1.0f);
    }
    m_last_scroll = ImGui::GetScrollY();
    m_last_max_scroll = ImGui::GetScrollMaxY();

    ImGui::EndChild();

    // FIXME: The prompt should be automatically refocused when the user presses any alphanumeric key or valid symbol used in aprompt.
    bool refocus_prompt = false;
    auto prompt_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
    if(ImGui::InputText("prompt", &m_prompt[0], (m_prompt.size()-1), prompt_flags)) {
      auto input = std::string(&m_prompt[0]);
      std::fill(m_prompt.begin(), m_prompt.end(), '\0');
      std::istringstream strm(input);
      std::vector<std::string> args;
      std::string arg;
      std::string command;
      std::getline(strm, command, ' ');
      while (std::getline(strm, arg, ' ')) {
        args.push_back(arg);
      }
      if(command.size() > 0) {
        if (auto found = m_command_list.find(command);
            found != m_command_list.end()) {
          found->second->execute(args);
        } else {
          m_output << "Invalid command!\n";
        }
      }
      // Scroll to bottom when prompt is submited.
      m_last_scroll = m_last_max_scroll;
      refocus_prompt = true;
    }

    ImGui::SetItemDefaultFocus();

    if(refocus_prompt) {
      ImGui::SetKeyboardFocusHere(-1);
    }

  }
  ImGui::End();
}

std::streamsize Console::xsputn(const char_type* s, std::streamsize count) {
  m_output.write(s, count);
  return count;
}
