#ifndef __BALL_H__
#define __BALL_H__

#include <sprite.h>

class Ball {
public:

  Ball();
  ~Ball();

  void update(float dt);
  void move(const Vec2& from, float dt);

  void disableCollisions();
  bool collisionEnabled() const;

  Sprite m_sprite;
  Vec2 m_velocity;
  Vec2 m_last_position;
  float m_speed = 0.0f;
  float m_angular_speed = 0.001f;

private:
	float m_reset_collision_dt = 0.0f;
	float m_reset_collision_time = 100.0f; // 100ms
	bool m_can_collide = true;
};

#endif // __BALL_H__