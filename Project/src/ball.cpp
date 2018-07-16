#include <ball.h>

#include <game.h>

Ball::Ball() {
  m_speed = 0.001f;
  m_velocity.x = (float)RandomNumber(0, 100) * 0.01f;
  m_velocity.y = (float)RandomNumber(0, 100) * 0.01f;

  float aspect_ratio = (float)Game::m_render_height / (float)Game::m_render_width;

  uint8_t* texture_data = m_sprite.getTextureData();
  CreateCircleInTexture(texture_data, m_sprite.m_texture_width,
    m_sprite.m_texture_height, Color(255, 255, 255, 255));
  
  m_sprite.setTextureData(texture_data);

  //m_sprite.setScaleY(aspect_ratio);
  m_sprite.m_scale.x = 0.03f;
  m_sprite.m_scale.y = 0.045f;
}

Ball::~Ball() {

}

void Ball::update(float dt) {
  Vec3 pos = m_sprite.getPosition();
  Vec3 scale = m_sprite.getScale();
  if (pos.x - scale.x * 0.5f < -1.0f || pos.x + scale.x * 0.5f > 1.0f) {
    m_velocity.x = -m_velocity.x;
    m_speed *= 1.05f;
  }
  if (pos.y - scale.y * 0.5f < -1.0f || pos.y + scale.y * 0.5f > 1.0f) {
    m_velocity.y = -m_velocity.y;
    m_speed *= 1.05f;
  }

  m_sprite.m_position += Vec3(m_velocity * m_speed * dt, 0.0f);
}