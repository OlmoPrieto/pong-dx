#include <ball.h>

#include <game.h>

Ball::Ball() {
  m_speed = 0.5f;
  //m_velocity.x = (float)RandomNumber(0, 100) * 0.01f;
  //m_velocity.y = (float)RandomNumber(0, 100) * 0.01f;
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

  m_sprite.m_position.x = (float)Game::m_render_width * 0.5f + 20.0f;
  m_sprite.m_position.y = (float)Game::m_render_height * 0.5f;
}

Ball::~Ball() {

}

void Ball::update(float dt) {
  if (m_can_collide == false &&
    Game::Instance()->msSinceStart() - m_reset_collision_dt >= m_reset_collision_time) {

    m_can_collide = true;
  }

  Vec2 pos = m_sprite.getPosition();
  Vec2 scale = m_sprite.getScale();

  // Check bounce with walls
  if (pos.x - scale.x < 0.0f || pos.x + scale.x > Game::m_render_width) {
    m_velocity.x = -m_velocity.x;
  }
  if (pos.y - scale.y < 0.0f || pos.y + scale.y > Game::m_render_height) {
    m_velocity.y = -m_velocity.y;
  }
  
  m_last_position = m_sprite.m_position;
  // Update movement
  move(m_sprite.m_position, dt);

  //m_last_position = m_sprite.m_position;
}

void Ball::move(const Vec2& from, float dt) {
  Vec2 radius = m_sprite.m_position - Vec2((float)Game::m_render_width * 0.5f,
    (float)Game::m_render_height * 0.5f);

  m_velocity.x += -m_angular_speed * radius.y;
  m_velocity.y +=  m_angular_speed * radius.x;

  m_sprite.m_position = m_velocity * m_speed * dt + from;

  //printf("pos: %.2f  %.2f\n", m_sprite.m_position.x, m_sprite.m_position.y);
  printf("as: %f\n", m_angular_speed);

  static float time = 0.0f;
  time += dt;
  
  if (m_angular_speed >= -0.000001f && m_angular_speed <= 0.000001f) {
    m_angular_speed = 0.0f;
    time = 0.0f;
  }
  else if (m_angular_speed != 0.0f) {
    m_angular_speed = Lerp(m_angular_speed, 0.0f, time * 0.001f);
    //m_angular_speed *= 0.0001f;
  }

  // if (m_angular_speed != 0.0f) {
  //   m_angular_speed = Lerp(m_angular_speed, 0.0f, time * 0.001f);
  //   //m_angular_speed *= 0.0001f;
  // }
  // else if (m_angular_speed >= -0.000001f && m_angular_speed <= 0.000001f) {
  //   m_angular_speed = 0.0f;
  //   time = 0.0f;
  //   printf("PINPIN\n");
  // }
  
}

void Ball::disableCollisions() {
  m_can_collide = false;
  m_reset_collision_dt = Game::Instance()->msSinceStart();
}

bool Ball::collisionEnabled() const {
  return m_can_collide;
}