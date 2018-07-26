#include <ball.h>

#include <game.h>

Ball::Ball() {
  m_speed = 0.5f;
  // m_velocity.x = (float)RandomNumber(0, 100) * 0.01f;
  // m_velocity.y = (float)RandomNumber(0, 100) * 0.01f;
  m_velocity.x = 1.0f;
  m_velocity.y = 0.0f;

  uint8_t* texture_data = m_sprite.getTextureData();
  CreateCircleInTexture(texture_data, m_sprite.m_texture_width,
    m_sprite.m_texture_height, Color(255, 255, 255, 255));
  
  m_sprite.setTextureData(texture_data);

  float scale = 0.0075f;
  if (Game::m_render_width > Game::m_render_height) {
    scale *= Game::m_render_width;
  }
  else {
    scale *= Game::m_render_height;
  }

  m_sprite.m_scale.x = scale;
  m_sprite.m_scale.y = scale;

  m_sprite.m_position.x = Game::m_render_width * 0.5f;
  m_sprite.m_position.y = Game::m_render_height * 0.5f;
}

Ball::~Ball() {

}

void Ball::update(float dt) {
  Vec2 pos = m_sprite.getPosition();
  Vec2 scale = m_sprite.getScale();

  // Check bounce with walls
  if (pos.x - scale.x < 0.0f || pos.x + scale.x > Game::m_render_width) {
    m_velocity.x = -m_velocity.x;
    m_speed *= 1.05f;
  }
  if (pos.y - scale.y < 0.0f || pos.y + scale.y > Game::m_render_height) {
    m_velocity.y = -m_velocity.y;
    m_speed *= 1.05f;
  }
  
  m_last_position = m_sprite.m_position;
  // Update movement
  //m_sprite.m_position += m_velocity * m_speed * dt;
  move(m_sprite.m_position, dt);

  //m_last_position = m_sprite.m_position;
}

void Ball::move(const Vec2& from, float dt) {
  m_sprite.m_position = m_velocity * m_speed * dt + from;
}