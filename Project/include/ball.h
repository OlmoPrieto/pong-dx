#ifndef __BALL_H__
#define __BALL_H__

#include <sprite.h>

class Ball {
public:

  Ball();
  ~Ball();

  void update(float dt);

  Sprite m_sprite;
  Vec2 m_velocity;
  float m_speed = 0.0f;

private:
};

#endif // __BALL_H__