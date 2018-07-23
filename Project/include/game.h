#ifndef __GAME_H__
#define __GAME_H__

#ifdef __PLATFORM_MACOSX__
  #include <OpenGL/gl3.h>
#elif __PLATFORM_LINUX__
  #include <glew/include/GL/glew.h>
#elif __PLATFORM_WINDOWS__
  #include <glew/include/GL/glew.h>
#elif __PLATFORM_ANDROID__
  #include <GLES2/gl2.h>
#endif

#include <ball.h>
#include <paddle.h>
#include <sprite.h>
#include <utils.h>

#include <vector>

class Game {
public:
  ~Game();
  Game(const Game& other) = delete;
  void operator = (const Game& other) = delete;

  static Game* Instance();
  static void setRenderSize(uint32_t width, uint32_t height);

  void update(float dt);
  void draw();

  static Vec2 m_player_starting_position;

  static uint32_t m_render_width;
  static uint32_t m_render_height;

private:
  struct OpenGLData {
    Mat4 m_projection;

    Vec3 m_vertices[4];
    Vec3 m_uvs[4];

    uint32_t m_indices[6];

    GLuint m_vao_id = 0;
    GLuint m_vertices_index = 0;
    GLuint m_indices_index = 0;
    GLuint m_program_id = 0;
    GLint m_mvp_location = -1;
    GLint m_color_location = -1;
    GLint m_position_location = -1;
    GLint m_uvs_location = -1;
  };

  Game();

  void init();
  void setupOpenGL();
  void drawSprite(Sprite* sprite);

  OpenGLData m_opengl_data;
  std::vector<Ball> m_balls;
  Paddle m_player;
};

#endif // __GAME_H__