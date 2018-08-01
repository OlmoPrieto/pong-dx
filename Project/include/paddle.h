#ifndef __PADDLE_H__
#define __PADDLE_H__

#include <ball.h>
#include <sprite.h>

class Paddle {
public:

  Paddle();
  ~Paddle();

  bool checkCollision(Ball* ball);
  void placeBallAtCollisionPoint(Ball* ball);

  void update(float dt);

  Sprite m_sprite;
  Vec2 m_last_position;
  float m_y_velocity = 0.0f;
  bool m_player_controlled = false;

private:
};

#endif // __PADDLE_H__