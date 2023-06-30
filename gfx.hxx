#ifndef GL_HXX
#define GL_HXX

namespace gfx {

class GraphicalSettings {
public:
  GraphicalSettings();
  GraphicalSettings(int res[2]);
  GraphicalSettings(int ww, int wh);
  GraphicalSettings(const GraphicalSettings &other);

  void set_resolution(int w, int h);
  void set_resolution(int resolution[2]);

  bool has_changed() const;
  void acknowledge_change();

public:
  int m_window_res[2];
  bool m_has_changed;
};

class Graphics {

  Graphics();
  Graphics(GraphicalSettings settings);

  void init();
  void destroy();

  GraphicalSettings get_settings() const;
  void update_settings(GraphicalSettings settings);
  bool has_settings_changed() const;

private:
  GraphicalSettings m_settings;
};

} // namespace gfx

#endif // GL_HXX
