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

  m_sprite.m_position = Game::m_player_starting_position;
}

Paddle::~Paddle() {

}

bool Paddle::checkCollision(Ball* ball) {
  Vec2 pos = m_sprite.m_position;
  Vec2 scale = m_sprite.m_scale;
  Vec2 other_pos = ball->m_sprite.m_position;
  Vec2 other_scale = ball->m_sprite.m_scale;

  // Vec2 pos = m_sprite.getPosition();
  // Vec2 scale = m_sprite.getScale();
  // Vec2 other_pos = ball->m_sprite.getPosition();
  // Vec2 other_scale = ball->m_sprite.getScale();

  // float d_x = fabs(pos.x - other_pos.x);
  // float d_y = fabs(pos.y - other_pos.y);

  // if (d_x > (other_scale.x + scale.x) || d_y > (other_scale.y + scale.y)) {
  //     return false;
  // }

  // if (d_x <= other_scale.x || d_y <= other_scale.y) {
  //     return true;
  // }

  // float corner_d_sq = (d_x - other_scale.x) * (d_x - other_scale.x) +
  //                     (d_y - other_scale.y) * (d_y - other_scale.y);

  // if (corner_d_sq <= scale.x * scale.x) {
  //     return true;
  // }

  // return false;

  // https://yal.cc/rectangle-circle-intersection-test/
  float d_x = other_pos.x - Max(pos.x - scale.x, Min(other_pos.x, pos.x + scale.x));
  float d_y = other_pos.y - Max(pos.y - scale.y, Min(other_pos.y, pos.y + scale.y));
  return (d_x * d_x + d_y * d_y) < (other_scale.x * other_scale.x);
}

void Paddle::placeBallAtCollisionPoint(Ball* ball) {
  float last_dt_percentage = Game::Instance()->m_last_frame_time * 0.1f;

  // [[ Method to take the ball back two steps and then apply collision ]]
  
  // Vec2 prev_vel = ball->m_velocity;
  // Vec2 vel = prev_vel;
  // vel *= -1.0f;
  // ball->move(ball->m_last_position, last_dt_percentage);
  // ball->move(ball->m_sprite.m_position, last_dt_percentage);

  // ball->m_velocity = prev_vel;
  // //ball->move(ball->m_last_position, last_dt_percentage);
  // while (!checkCollision(ball)) {
  //   ball->move(ball->m_sprite.m_position, last_dt_percentage);
  // }



  // [[ Naive implementation ]]
  
  ball->move(ball->m_last_position, last_dt_percentage);
  while (!checkCollision(ball)) {
    ball->move(ball->m_sprite.m_position, last_dt_percentage);
  }
}

void Paddle::update(float dt) {

}