#ifndef __INPUT_H__
#define __INPUT_H__

#include <utils.h>

#include <deque>

class Input {
public:
  struct Event {
    enum Type {
      None = 0,
      Down = 1,
      Up = 2,
      Move = 3
    } type ;

    float x;
    float y;
    float time_stamp;

    Event() : x(0.0f), y(0.0f), time_stamp(0.0f) {}
    Event(float x, float y, float time_stamp, Type type) :
      x(x), y(y), time_stamp(time_stamp), type(type) {}
  };

  ~Input();

  static void registerEvent(const Event& event);
  static void registerEvent(float x, float y, float time_stamp, int32_t type);

  static Vec2 getCursorPos();
  static bool tapped();
  static bool isScreenPressed();

private:
  Input();

  static std::deque<Event> m_events;
  static bool m_screen_pressed;
  static bool m_begin_tap;
  static uint32_t m_begin_tap_index;
  static uint32_t m_end_tap_index;
};

#endif // __INPUT_H__