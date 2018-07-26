#ifndef __BALL_H__
#define __BALL_H__

#include <sprite.h>

class Ball {
public:

  Ball();
  ~Ball();

  void update(float dt);
  void move(const Vec2& from, float dt);

  Sprite m_sprite;
  Vec2 m_velocity;
  Vec2 m_last_position;
  float m_speed = 0.0f;

private:
};

#endif // __BALL_H__