#include <iostream>

#include <chrono.h>
#include <game.h>
#include <input.h>

#ifdef __PLATFORM_MACOSX__
  #include <OpenGL/gl3.h>
#elif __PLATFORM_LINUX__
  #include <glew/include/GL/glew.h>
#elif __PLATFORM_WINDOWS__
  #include <glew/include/GL/glew.h>
#endif
#include <GLFW/include/glfw3.h>

typedef unsigned char byte;

// GLOBAL VARIABLES
GLFWwindow* g_window = nullptr;
uint32_t g_window_width  = 720;
uint32_t g_window_height = 480;


static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

void InitializeGraphics() {
  printf("Initializing graphic context...\n");

  if (!glfwInit()) {
    printf("Failed to initialize GLFW\n");
    exit(1);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
  #ifdef __PLATFORM_MACOSX__
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  #endif

  g_window_width = Game::m_render_width;
  g_window_height = Game::m_render_height;
  g_window = glfwCreateWindow(g_window_width, g_window_height, "Window", NULL, NULL);
  if (!g_window) {
    glfwTerminate();
    printf("Failed to create window\n");
    exit(1);
  }

  glfwMakeContextCurrent(g_window);
  //glfwSwapInterval(0); // comment/uncomment to enable/disable 'vsync'
  glfwSetKeyCallback(g_window, KeyCallback);

  #ifdef __PLATFORM_LINUX__
    glewInit();
  #elif __PLATFORM_WINDOWS__
    glewInit();
  #endif
}

int main() {
  InitializeGraphics();

  Game* game = Game::Instance();

  // TODO: if making a pause mode in Game, make the cursor
  // to reappear when pausing the game and to hide
  // when resuming the game
  // -----------------------------------------------------------
  //glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

  Chrono c;
  double prev_x_pos = 0.0, prev_y_pos = 0.0;
  double x_pos = 0.0, y_pos = 0.0;
  while (!glfwWindowShouldClose(g_window)) {
    glfwPollEvents();

    // ============== | Query input and register glfw Events | ===============
    glfwGetCursorPos(g_window, &x_pos, &y_pos);

    int32_t state = glfwGetMouseButton(g_window, GLFW_MOUSE_BUTTON_LEFT);
    Input::Event::Type event_type = Input::Event::Type::None;
    if (state == GLFW_PRESS) {
      event_type = Input::Event::Type::Down;
    }
    else if (state == GLFW_RELEASE) {
      event_type = Input::Event::Type::Up;
    }

    if (event_type != Input::Event::Type::None) {
      Input::registerEvent((float)x_pos, (float)y_pos, game->msSinceStart(),
        event_type);
    }

    if (x_pos != prev_x_pos || y_pos != prev_y_pos) {
      Input::registerEvent((float)x_pos, (float)y_pos, game->msSinceStart(), 
        Input::Event::Type::Move);
    }

    prev_x_pos = x_pos;
    prev_y_pos = y_pos;
    // =======================================================================


    game->draw();

    glfwSwapBuffers(g_window);
    //glfwPollEvents();
  }

  return 0;
}