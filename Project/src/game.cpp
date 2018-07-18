#include <game.h>
#include <iostream>

// ========================================================================
static bool CheckGLError(const char* tag = "") {
  GLenum error = glGetError(); // pops the last error from the queue
  if (error != GL_NO_ERROR) {
    switch (error) {
      case GL_INVALID_OPERATION: {
        printf("%s : Invalid operation\n", tag);
        break;
      }
      case GL_INVALID_VALUE: {
        printf("%s : Invalid value\n", tag);
        break;
      }
      case GL_INVALID_ENUM: {
        printf("%s : Invalid enum\n", tag);
        break;
      }
      case GL_OUT_OF_MEMORY: {
        printf("%s : Out of memory\n", tag);
        break;
      }
      default: {
        printf("Not handled: %s\n", tag);
        break;
      }
    }
  }

  return error != GL_NO_ERROR;
}

static const char* vertex_shader_code = 
#ifdef __PLATFORM_ANDROID__
R"END(
  #version 100
  precision mediump float;
)END"
#else
R"END(
  #version 110
)END"
#endif
R"END(
  attribute vec3 position;
  attribute vec3 uvs;
  uniform mat4 MVP;
  varying vec2 o_uv;

  void main() {
    gl_Position = MVP * vec4(position, 1.0);
    o_uv = vec2(uvs.x, uvs.y);
  }
)END";

static const char* fragment_shader_code = 
#ifdef __PLATFORM_ANDROID__
R"END(
  #version 100
  precision mediump float;
)END"
#else
R"END(
  #version 110
)END"
#endif
R"END(
  uniform vec4 color;
  uniform sampler2D tex;
  varying vec2 o_uv;

  void main() {
    //gl_FragColor = color * vec4(o_uv.x, o_uv.y, 0.0, 1.0);
)END"
//#ifdef __PLATFORM_ANDROID__
R"END(
    gl_FragColor = texture2D(tex, o_uv) * color;
)END"
// #else
// R"END(
//     gl_FragColor = texture(tex, o_uv) * color;
// )END"
// #endif
R"END(
  }
)END";
// ========================================================================

uint32_t Game::m_render_width  = 1080;
uint32_t Game::m_render_height = 720;
// uint32_t Game::m_render_width  = 480;
// uint32_t Game::m_render_height = 640;

uint32_t Game::m_render_desired_width = 720;
uint32_t Game::m_render_desired_height = 480;

Game::~Game() {

}

Game* Game::Instance() {
  static Game game;

  return &game;
}

void Game::update(float dt) {
  for (uint32_t i = 0; i < m_balls.size(); ++i) {
    m_balls[i].update(16.0f);
  }
}

void Game::draw() {
  update(16.0f);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // [RENDER TEST]
  // glUniform4f(m_opengl_data.m_color_location, 1.0f, 0.0f, 0.0f, 1.0f);

  // glBindBuffer(GL_ARRAY_BUFFER, m_opengl_data.m_vertices_index);
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_opengl_data.m_indices_index);

  // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, m_opengl_data.m_indices);
  
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  // glBindBuffer(GL_ARRAY_BUFFER, 0);
  // [\RENDER TEST]

  for (uint32_t i = 0; i < m_balls.size(); ++i) {
    drawSprite(&m_balls[i].m_sprite);
  }
}

void Game::drawSprite(Sprite* sprite) {
  Color c = sprite->getColor();
  glUniform4f(m_opengl_data.m_color_location, (float)c.r / (float)255, 
    (float)c.g / (float)255, (float)c.b / (float)255, (float)c.a / (float)255);

  glBindTexture(GL_TEXTURE_2D, sprite->getTextureHandler());

  glBindBuffer(GL_ARRAY_BUFFER, m_opengl_data.m_vertices_index);

  Vec3 scale = sprite->getScale();
  Vec3 pos = sprite->getPosition();
  Vec3 vertices[4] = {
    {
      m_opengl_data.m_vertices[0].x * scale.x + pos.x, 
      m_opengl_data.m_vertices[0].y * scale.y + pos.y, 
      m_opengl_data.m_vertices[0].z * scale.z + pos.z 
    },
    {
      m_opengl_data.m_vertices[1].x * scale.x + pos.x, 
      m_opengl_data.m_vertices[1].y * scale.y + pos.y, 
      m_opengl_data.m_vertices[1].z * scale.z + pos.z 
    },
    {
      m_opengl_data.m_vertices[2].x * scale.x + pos.x, 
      m_opengl_data.m_vertices[2].y * scale.y + pos.y, 
      m_opengl_data.m_vertices[2].z * scale.z + pos.z 
    },
    { 
      m_opengl_data.m_vertices[3].x * scale.x + pos.x, 
      m_opengl_data.m_vertices[3].y * scale.y + pos.y, 
      m_opengl_data.m_vertices[3].z * scale.z + pos.z 
    },
  };
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_opengl_data.m_indices_index);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, m_opengl_data.m_indices);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

Game::Game() {
  init();
}

void Game::init() {
  // IMPORTANT! Always first
  setupOpenGL();

  // Game stuff
  m_balls.reserve(10);  // TODO: max 10 balls at the same time?
  m_balls.emplace_back();
}


void Game::setupOpenGL() {
  printf("Initializing OpenGL\n");

  #ifndef __PLATFORM_ANDROID__
    glGenVertexArrays(1, &m_opengl_data.m_vao_id);
    CheckGLError("glGenVertexArrays");
    printf("vao id: %u\n", m_opengl_data.m_vao_id);
    glBindVertexArray(m_opengl_data.m_vao_id);
  #endif

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  m_opengl_data.m_vertices[0] = { -1.0f,  1.0f, 0.0f };
  m_opengl_data.m_vertices[1] = { -1.0f, -1.0f, 0.0f };
  m_opengl_data.m_vertices[2] = {  1.0f,  1.0f, 0.0f };
  m_opengl_data.m_vertices[3] = {  1.0f, -1.0f, 0.0f };

  m_opengl_data.m_uvs[0] = { 0.0f, 1.0f, 0.0f };
  m_opengl_data.m_uvs[1] = { 0.0f, 0.0f, 0.0f };
  m_opengl_data.m_uvs[2] = { 1.0f, 1.0f, 0.0f };
  m_opengl_data.m_uvs[3] = { 1.0f, 0.0f, 0.0f };

  m_opengl_data.m_indices[0] = 0;
  m_opengl_data.m_indices[1] = 1;
  m_opengl_data.m_indices[2] = 2;
  m_opengl_data.m_indices[3] = 3;
  m_opengl_data.m_indices[4] = 2;
  m_opengl_data.m_indices[5] = 1;

  glGenBuffers(1, &m_opengl_data.m_vertices_index);
  glBindBuffer(GL_ARRAY_BUFFER, m_opengl_data.m_vertices_index);
  glBufferData(GL_ARRAY_BUFFER, sizeof(m_opengl_data.m_vertices) + 
    sizeof(m_opengl_data.m_uvs) + sizeof(m_opengl_data.m_indices), 
    m_opengl_data.m_vertices, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(m_opengl_data.m_vertices), 
    sizeof(m_opengl_data.m_uvs), m_opengl_data.m_uvs);
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(m_opengl_data.m_vertices) + 
    sizeof(m_opengl_data.m_uvs), sizeof(m_opengl_data.m_indices), 
    m_opengl_data.m_indices);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Create and compile vertex shader
  GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader_id, 1, &vertex_shader_code, nullptr);
  glCompileShader(vertex_shader_id);
  GLint vertex_shader_compiling_success = 0;
  glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &vertex_shader_compiling_success);
  if (!vertex_shader_compiling_success) {
    printf("Failed to compile vertex shader\n");
    GLint log_size = 0;
    glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &log_size);
    char* log = (char*)malloc(log_size);
    GLint read = 0;
    glGetShaderInfoLog(vertex_shader_id, log_size, &read, log);
    printf("Error: %s\n", log);
    free(log);
  }

  // Create and compile fragment shader
  GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader_id, 1, &fragment_shader_code, nullptr);
  glCompileShader(fragment_shader_id);
  GLint fragment_shader_compiling_success = 0;
  glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &fragment_shader_compiling_success);
  if (!fragment_shader_compiling_success) {
    printf("Failed to compile fragment shader\n");
    GLint log_size = 0;
    glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &log_size);
    char* log = (char*)malloc(log_size);
    GLint read = 0;
    glGetShaderInfoLog(fragment_shader_id, log_size, &read, log);
    printf("Error: %s\n", log);
    free(log);
  }

  m_opengl_data.m_program_id = glCreateProgram();
  glAttachShader(m_opengl_data.m_program_id, vertex_shader_id);
  glAttachShader(m_opengl_data.m_program_id, fragment_shader_id);
  glLinkProgram(m_opengl_data.m_program_id);

  m_opengl_data.m_mvp_location = glGetUniformLocation(m_opengl_data.m_program_id, "MVP");
  m_opengl_data.m_color_location = glGetUniformLocation(m_opengl_data.m_program_id, "color");
  
  glBindBuffer(GL_ARRAY_BUFFER, m_opengl_data.m_vertices_index);
  m_opengl_data.m_position_location = glGetAttribLocation(m_opengl_data.m_program_id, "position");
  glEnableVertexAttribArray(m_opengl_data.m_position_location);
  glVertexAttribPointer(m_opengl_data.m_position_location, 3, GL_FLOAT, 
    GL_FALSE, 0, (GLvoid*)0);

  m_opengl_data.m_uvs_location = glGetAttribLocation(m_opengl_data.m_program_id, "uvs");
  glEnableVertexAttribArray(m_opengl_data.m_uvs_location);
  glVertexAttribPointer(m_opengl_data.m_uvs_location, 3, GL_FLOAT, GL_FALSE, 0, 
    (GLvoid*)sizeof(m_opengl_data.m_vertices));

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  //  Projection matrix
  //    column-major order!

  float right  =  1.0f;
  float left   = -1.0f;
  float top    =  1.0f;
  float bottom = -1.0f;

  float aspect_ratio = m_render_width / m_render_height;
  if (m_render_width > m_render_height) {
    aspect_ratio = m_render_width / m_render_height;
    right  = m_render_width;
    left   = 0.0f;
    top    = m_render_height;
    bottom = 0.0f;
  }
  else {
    aspect_ratio = m_render_height / m_render_width;
    right  = m_render_width;
    left   = 0.0f;
    top    = m_render_height;
    bottom = 0.0f;
  }

  float near = -1.0f;
  float far  = 1.0f;
  float fov  = 60.0f;

  m_opengl_data.m_projection.matrix[0] = 2.0f / (right - left);
  m_opengl_data.m_projection.matrix[1] = 0.0f;
  m_opengl_data.m_projection.matrix[2] = 0.0f;
  m_opengl_data.m_projection.matrix[3] = 0.0f;

  m_opengl_data.m_projection.matrix[4] = 0.0f;
  m_opengl_data.m_projection.matrix[5] = 2.0f / (top - bottom);
  m_opengl_data.m_projection.matrix[6] = 0.0f;
  m_opengl_data.m_projection.matrix[7] = 0.0f;

  m_opengl_data.m_projection.matrix[8]  = 0.0f;
  m_opengl_data.m_projection.matrix[9]  = 0.0f;
  m_opengl_data.m_projection.matrix[10] = -2.0f / (far - near);
  m_opengl_data.m_projection.matrix[11] = 0.0f;
  
  m_opengl_data.m_projection.matrix[12] = -(right + left) / (right - left);
  m_opengl_data.m_projection.matrix[13] = -(top + bottom) / (top - bottom);
  m_opengl_data.m_projection.matrix[14] = -(far + near  ) / (far - near  );
  m_opengl_data.m_projection.matrix[15] = 1.0f;

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  glUseProgram(m_opengl_data.m_program_id);
  glUniformMatrix4fv(m_opengl_data.m_mvp_location, 1, GL_FALSE, 
    (const GLfloat*)m_opengl_data.m_projection.matrix);
}















