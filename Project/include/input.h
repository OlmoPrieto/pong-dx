#ifndef __INPUT_H__
#define __INPUT_H__

#include <utils.h>

#include <queue>

class Input {
public:
  struct Event {
    float x;
    float y;
    float time_stamp;

    Event() : x(0.0f), y(0.0f), time_stamp(0.0f) {}
    Event(float x, float y, float time_stamp) :
      x(x), y(y), time_stamp(time_stamp) {}
  };

  ~Input();

  static Vec2 getCursorPos();
  static bool tapped();
  static bool isScreenPressed();

  static void registerEvent(const Event& event);

private:
  Input();

  std::queue<Event> m_events;
};

#endif // __INPUT_H__