#ifndef GFX_HXX
#define GFX_HXX

namespace gfx {

  /* NOTE: The graphical settings are stored in a separate class because
    I wanted to decouple where the settings from where its used. This way
   it can be modified somewhere else in the code and than simply applied to
  the graphics subsystem */
class GraphicalSettings {
public:
  GraphicalSettings();
  GraphicalSettings(int res[2]);
  GraphicalSettings(int ww, int wh);
  GraphicalSettings(const GraphicalSettings &other);

  void set_resolution(int w, int h);
  void set_resolution(int resolution[2]);

  void load_from_disk();
  void save_settings() const;

  bool has_changed() const;
  void acknowledge_change();

private:

  int m_window_res[2];
  bool m_has_changed;
};

class Graphics {
public:

  Graphics();
  Graphics(GraphicalSettings settings);
  ~Graphics();

  void init();
  void destroy();

  GraphicalSettings get_settings() const;
  void update_settings(GraphicalSettings settings);
  bool has_settings_changed() const;

private:
  GraphicalSettings m_settings;
};

} // namespace gfx

#endif // GFX_HXX
