#include "console.hxx"
#include <utility>
#include <imgui.h>

size_t m_console_count = 0;

Console::Console() {
    m_console_count += 1;
    std::stringstream strm;
    strm << "Console<" << m_console_count << ">";
    m_title = strm.str();
}

Console::~Console() {
    m_console_count -= 1;
}

Console::Console(Console&& other):
    m_history(std::move(other.m_history)),
    m_is_visible(std::move(other.m_is_visible))
{}
Console::Console(const Console& other)  :
    m_history(other.m_history),
    m_is_visible(other.m_is_visible)
{}


bool Console::visible() const {
    return m_is_visible;
}

void Console::toggle_visible(){
    m_is_visible = !m_is_visible;
}

void Console::hide(){
    m_is_visible = false;
}

void Console::show(){
    m_is_visible = true;
}

void Console::draw() {
    ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_FirstUseEver);
    if(!ImGui::Begin(m_title.c_str(), &m_is_visible)) {
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

    if(ImGui::BeginPopup("Options")) {
        ImGui::EndPopup();
    }

    if(ImGui::Button("Options")) {
        ImGui::OpenPopup("Options");
    }

    ImGui::End();
}
