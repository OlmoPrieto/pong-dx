#ifndef __GAME_H__
#define __GAME_H__

#ifndef __PLATFORM_ANDROID__
#include <OpenGL/gl3.h>
#else
#include <GLES2/gl2.h>
#endif

#include <utils.h>

class Game {
public:
  ~Game();
  Game(const Game& other) = delete;
  void operator = (const Game& other) = delete;

  static Game* Instance();

  void update(float dt);
  void draw();


private:
  Game();

  void init();
  void setupOpenGL();


  Mat4 m_projection;

  Vec3 m_vertices[4];
  Vec3 m_uvs[4];

  uint32_t m_indices[6];

  GLuint m_vao_id = 0;
  GLuint m_vertices_index = 0;
  GLuint m_indices_index = 0;
  GLint m_mvp_location = -1;
  GLint m_color_location = -1;
  GLint m_position_location = -1;
  GLint m_uvs_location = -1;
};

#endif // __GAME_H__