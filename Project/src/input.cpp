#include <input.h>

#include <game.h>

std::deque<Input::Event> Input::m_events;
bool Input::m_screen_pressed = false;

Input::Input() {
  // TODO: if implementing a custom queue, reserve space for elements here
}

Input::~Input() {

}

void Input::registerEvent(const Event& event) {
  m_events.push_back(event);
}

void Input::registerEvent(float x, float y, float time_stamp, int32_t type) {
  if ((Event::Type)type == Event::Type::Down) {
    m_screen_pressed = true;
  }
  else if ((Event::Type)type == Event::Type::Up) {
    m_screen_pressed = false;
  }

  m_events.emplace_back(x, y, time_stamp, (Event::Type)type);
}

Vec2 Input::getCursorPos() {
  Vec2 result;

  if (m_events.size() > 0) {
    Event e(m_events.back());
    result.x = e.x;
    result.y = Game::m_render_height - e.y;

    if (m_events.size() > 1000) {
      // Even though I'm clearing the events here, the flag
      // for if the screen is touched remains intact.
      m_events.clear();
    }
  }
  
  return result;
}

bool Input::tapped() {
  return false;
}

bool Input::isScreenPressed() {
  return m_screen_pressed;
}