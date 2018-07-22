#include <paddle.h>

#include <game.h>

Paddle::Paddle() {
  uint8_t* texture_data = m_sprite.getTextureData();
  SetTextureColor(texture_data, m_sprite.m_texture_width,
    m_sprite.m_texture_height, Color(255, 255, 255, 255));
  m_sprite.setTextureData(texture_data);

  float x_scale = (float)(Game::m_render_width) * 0.009f;
  float y_scale = (float)(Game::m_render_height) * 0.11f;

  m_sprite.m_scale.x = x_scale;
  m_sprite.m_scale.y = y_scale;

  printf("%.2f %.2f\n", Game::m_player_starting_position.x, Game::m_player_starting_position.y);
  m_sprite.m_position = Game::m_player_starting_position;
}

Paddle::~Paddle() {

}

bool Paddle::checkCollision(Ball* ball) {
  Vec2 pos = m_sprite.getPosition();
  Vec2 scale = m_sprite.getScale();
  Vec2 other_pos = ball->m_sprite.m_position;
  Vec2 other_scale = ball->m_sprite.m_scale;

  float d_x = fabs(pos.x - other_pos.x);
  float d_y = fabs(pos.y - other_pos.y);

  if (d_x > (other_scale.x + scale.x) || d_y > (other_scale.y + scale.y)) {
      return false;
  }

  if (d_x <= other_scale.x || d_y <= other_scale.y) {
      return true;
  }

  float corner_d_sq = (d_x - other_scale.x) * (d_x - other_scale.x) +
                      (d_y - other_scale.y) * (d_y - other_scale.y);

  if (corner_d_sq <= scale.x * scale.x) {
      return true;
  }

  return false;
}

void Paddle::update(float dt) {

}