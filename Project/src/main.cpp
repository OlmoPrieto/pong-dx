#include <iostream>

#include <chrono.h>
#include <game.h>

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

  Chrono c;
  while (!glfwWindowShouldClose(g_window)) {
    c.start();

    game->draw();

    glfwSwapBuffers(g_window);
    glfwPollEvents();

    c.stop();
    //printf("Frame time: %.2f ms\n", c.timeAsMilliseconds());
  }

  return 0;
}