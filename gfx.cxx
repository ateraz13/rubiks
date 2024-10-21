#include "gfx.hxx"
#include <cstring>
#include <utility>

using namespace gfx;

GraphicalSettings::GraphicalSettings() : m_window_res {800, 600} {}

GraphicalSettings::GraphicalSettings(int res[2])
    : m_window_res {res[0], res[1]} {}

GraphicalSettings::GraphicalSettings(int ww, int wh) : m_window_res {ww, wh} {}

// FIXME: Check if settings are equal if yes than return don't update "changed"
// status.
GraphicalSettings::GraphicalSettings(const GraphicalSettings &other) {
  std::memcpy(m_window_res, other.m_window_res, sizeof(m_window_res));
  m_has_changed = true;
}

void GraphicalSettings::set_resolution(int res[2]) {
  m_has_changed = true;
  m_window_res[0] = res[0];
  m_window_res[1] = res[1];
}

void GraphicalSettings::set_resolution(int ww, int wh) {
  m_has_changed = true;
  m_window_res[0] = ww;
  m_window_res[1] = wh;
}

bool GraphicalSettings::has_changed() const { return m_has_changed; }

void GraphicalSettings::acknowledge_change() { m_has_changed = false; }

Graphics::Graphics() : m_settings() {}

Graphics::Graphics(GraphicalSettings settings)
    : m_settings(std::move(settings)) {}

Graphics::~Graphics() { destroy(); }

void Graphics::init() {}

void Graphics::destroy() {}

GraphicalSettings Graphics::get_settings() const { return m_settings; }

void Graphics::update_settings(GraphicalSettings settings) {
  m_settings = settings;
}
