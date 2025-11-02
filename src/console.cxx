#include <rubiks/console.hxx>
#include <functional>
#include <imgui.h>
#include <iostream>
#include <utility>

size_t m_console_count = 0;

Console::Console() {
  m_console_count += 1;
  std::stringstream strm;
  strm << "Console<" << m_console_count << ">";
  m_title = strm.str();
  m_command_list["quit"] = Command([](auto args) {
    QuitAction action;
    action.execute();
  }).with_description("Quit the game.");

  m_command_list["print"] = Command([&](auto args) {
    for(auto arg: args) {
      m_output << arg << " ";
    }
    m_output << "\n";
  }).with_description("Print text into the console.");

  m_command_list["help"] = Command([&](auto args) {
    // TODO: Implement sub commands for help with individual commands
    for(auto cmd: m_command_list) {
      m_output << cmd.first << ": " << cmd.second.description() << "\n";
    }
  }).with_description("Print this help message.");

  m_command_list["clear"] = Command([&](auto args) {
    m_output.str("");
  }).with_description("Clear the console.");
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
          found->second.execute(args);
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

console_lang::Scope::Scope() {}
console_lang::Scope::Scope(const console_lang::Scope& other) {}
console_lang::Scope::Scope(console_lang::Scope&& other) {}
console_lang::Scope::~Scope() {}

void console_lang::Scope::attach_parent_scope(console_lang::Scope* parent_scope) {
  m_parent_scope = parent_scope;
}

std::optional<std::reference_wrapper<CVal>> console_lang::Scope::find_binding(const std::string &name) {
  if(auto found = m_local_vars.find(name);
     found != m_local_vars.end()) {
    return found->second;
  } else {
    if(m_parent_scope) {
      return m_parent_scope->find_binding(name);
    } else {
      return std::nullopt;
    }
  }
}

void console_lang::Interp::run_code(const char *str, size_t count, Console &output) {

}
