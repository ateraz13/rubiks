#include "game.hxx"
#include "except.hxx"
#include "gl_calls.hxx"
#include "utility.hxx"
#include "window.hxx"
#include <GLFW/glfw3.h>
#include <chrono>
#include <cmath>
#include <cstring>
#include <functional>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <thread>

#define DEBUG_MESSAGES

const int MAIN_WINDOW_DEFAULT_HEIGHT = 768;
const int MAIN_WINDOW_DEFAULT_WIDTH = 1024;
const char *MAIN_WINDOW_DEFAULT_TITLE = "Rubiks";

Game &Game::instance() {
  static auto &game = Game::init();
  return game;
}

void glfw_error_callback(int error, const char *desc) {
  fprintf(stderr, "GLFW Error: %s\n", desc);
}

Game::Game() {}

Game &Game::init() {
  static Game game;
  std::cout << "Initialising game!\n";
  game.init_window_system();
  game.init_input_system();
  game.m_gfx.init();

  return game;
}

Game::~Game() {
  std::clog << "Destroying game!\n";
  // save("before_exit_autosave");
}

void Game::acknowledge_main_window_resize(SystemWindow win, int width,
                                          int height) {
  auto &inst = Game::instance();
  std::cout << "Window resized: width = " << width << ", height = " << height
            << "\n";
  win.bind_context();
  inst.m_gfx.viewport_size(width, height);
}

void QuitAction::operator()() { Game::instance().stop(); }

void Game::init_input_system() {

  // FIXME: Make sure the input_handler is never called by GLFW after Game has
  // been freed.
  auto key_event =
      KeyEvent{*m_main_window, GLFW_KEY_ESCAPE, KeyState::RELEASED};
  m_keymap[key_event] = std::make_unique<QuitAction>();

  for (auto &[ev, ac] : m_keymap) {
    std::cout << "MAP KEY: " << ev.key << " ACTION: " << ac << std::endl;
  }
}

void Game::init_window_system() {

  //   glfwSetWindowSizeCallback(mw, main_window_resized_cb);
  m_main_window =
      WindowSystem::instance()
          .new_window("main-window")
          .with_size(MAIN_WINDOW_DEFAULT_WIDTH, MAIN_WINDOW_DEFAULT_HEIGHT)
          .with_title("Rubiks!")
          .with_opengl(4, 5)
          .with_imgui()
          .build();

  using namespace std::placeholders;
  m_main_window->resize_cb = Game::acknowledge_main_window_resize;
  m_main_window->bind_context();


  glfwSwapInterval(0);
  m_gfx.viewport_size(MAIN_WINDOW_DEFAULT_WIDTH, MAIN_WINDOW_DEFAULT_HEIGHT);
}

void Game::save(const std::string &name) {
  // TODO: Save the game
  throw NotImplemented("Save functionality is not implemented!\n");
}

void Game::update_current_time() {
  m_last_time = m_current_time;
  m_current_time = glfwGetTime(); // FIXME: Update current time!
  uint64_t delta_time = m_current_time - m_last_time;
}

void Game::start() {

  if (m_is_running) {
    throw std::runtime_error("Game is already running!");
  }

  m_is_running = true;
  while (m_is_running) {
    update_current_time();
    update();
  }
}

void Game::stop() { m_is_running = false; }

void Game::update() {

  // while (m_action_queue.size() > 0) {
  //   auto action = m_action_queue.front();

  //   switch (action) {
  //   case QUIT_GAME:
  //     std::cout << "Quiting game!\n";
  //     this->stop();
  //     break;
  //   case ROTATE_1ST_COLUMN_FORWARD:
  //     m_rcube.rotate_1st_column_forward();
  //     break;
  //   case ROTATE_2ND_COLUMN_FORWARD:
  //     m_rcube.rotate_2nd_column_forward();
  //     break;
  //   case ROTATE_3RD_COLUMN_FORWARD:
  //     m_rcube.rotate_3rd_column_forward();
  //     break;
  //   case ROTATE_1ST_COLUMN_BACKWARDS:
  //     m_rcube.rotate_1st_column_backwards();
  //     break;
  //   case ROTATE_2ND_COLUMN_BACKWARDS:
  //     m_rcube.rotate_2nd_column_backwards();
  //     break;
  //   case ROTATE_3RD_COLUMN_BACKWARDS:
  //     m_rcube.rotate_3rd_column_backwards();
  //     break;
  //   case ROTATE_1ST_ROW_FORWARD:
  //     m_rcube.rotate_1st_row_forward();
  //     break;
  //   case ROTATE_2ND_ROW_FORWARD:
  //     m_rcube.rotate_2nd_row_forward();
  //     break;
  //   case ROTATE_3RD_ROW_FORWARD:
  //     m_rcube.rotate_3rd_row_forward();
  //     break;
  //   case ROTATE_1ST_ROW_BACKWARDS:
  //     m_rcube.rotate_1st_row_backwards();
  //     break;
  //   case ROTATE_2ND_ROW_BACKWARDS:
  //     m_rcube.rotate_2nd_row_backwards();
  //     break;
  //   case ROTATE_3RD_ROW_BACKWARDS:
  //     m_rcube.rotate_3rd_row_backwards();
  //     break;
  //   default:
  //     break;
  //   }

  //   m_action_queue.pop();
  // }

  using namespace std::chrono;
  auto frame_begin_time = steady_clock::now();

  WindowSystem::poll_events();
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  glm::vec3 clear_color = {1.0f, 0.0f, 0.0f};

  // 2. Show a simple window that we create ourselves. We use a Begin/End pair
  // to create a named window.
  {
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and
                                   // append into it.

    ImGui::Text("This is some useful text."); // Display some text (you can use
                                              // a format strings too)

    ImGui::SliderFloat("float", &f, 0.0f,
                       1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::ColorEdit3(
        "clear color",
        (float *)&clear_color); // Edit 3 floats representing a color

    if (ImGui::Button("Button")) // Buttons return true when clicked (most
                                 // widgets return true when edited/activated)
      counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    // ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
    //             1000.0f / io.Framerate, io.Framerate);
    ImGui::End();
  }

  ImGui::Render();
  m_main_window->bind_context();
  glClearColor(0.12f, 0.0, 0.12f, 1.0f);
  glClearDepth(10.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDisable(GL_CULL_FACE);
  glDisable(GL_SCISSOR_TEST);
  // glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_gfx.draw();
  m_main_window->swap_buffers();

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  using namespace std::chrono_literals;
  using std::chrono::duration;

  auto framerate = 60;

  auto elapsed_duration =
      duration_cast<milliseconds>(milliseconds(1000 / framerate) -
                                  (frame_begin_time - m_last_frame_timepoint));

  auto sleep_duration = std::max(elapsed_duration, 0ms);
  std::this_thread::sleep_for(sleep_duration);
  m_last_frame_timepoint = frame_begin_time;
}

void RubiksCube::rotate_1st_column_forward() {
  std::cout << "rotate_1st_column_forward" << std::endl;
}

void RubiksCube::rotate_2nd_column_forward() {
  std::cout << "rotate_2nd_column_forward" << std::endl;
}

void RubiksCube::rotate_3rd_column_forward() {
  std::cout << "rotate_3rd_column_forward" << std::endl;
}

void RubiksCube::rotate_1st_column_backwards() {
  std::cout << "rotate_1st_column_backwards" << std::endl;
}

void RubiksCube::rotate_2nd_column_backwards() {
  std::cout << "rotate_2nd_column_backwards" << std::endl;
}

void RubiksCube::rotate_3rd_column_backwards() {
  std::cout << "rotate_3rd_column_backwards" << std::endl;
}

void RubiksCube::rotate_1st_row_forward() {
  std::cout << "rotate_1st_row_forward" << std::endl;
}

void RubiksCube::rotate_2nd_row_forward() {
  std::cout << "rotate_2nd_row_forward" << std::endl;
}

void RubiksCube::rotate_3rd_row_forward() {
  std::cout << "rotate_3rd_row_forward" << std::endl;
}

void RubiksCube::rotate_1st_row_backwards() {
  std::cout << "rotate_1st_row_backwards" << std::endl;
}

void RubiksCube::rotate_2nd_row_backwards() {
  std::cout << "rotate_2nd_row_backwards" << std::endl;
}

void RubiksCube::rotate_3rd_row_backwards() {
  std::cout << "rotate_3rd_row_backwards" << std::endl;
}

void RubiksCube::reset() { std::cout << "reset" << std::endl; }

double Game::current_time() const { return m_current_time; }
