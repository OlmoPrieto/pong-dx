#include <input.h>

#include <game.h>

std::deque<Input::Event> Input::m_events;
bool Input::m_screen_pressed = false;
bool Input::m_begin_tap = false;
uint32_t Input::m_begin_tap_index = 0;
uint32_t Input::m_end_tap_index = 0;

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
    if (m_screen_pressed == false) {
      m_begin_tap = true;
      m_begin_tap_index = m_events.size();
    }
    m_screen_pressed = true;
  }
  else if ((Event::Type)type == Event::Type::Up) {
    m_screen_pressed = false;
    if (m_begin_tap == true) {
      m_end_tap_index = m_events.size();
    }
    m_begin_tap = false;
  }

  m_events.emplace_back(x, y, time_stamp, (Event::Type)type);
}

Vec2 Input::getCursorPos() {
  Vec2 result;

  if (m_events.size() > 0) {
    Event e(m_events.back());
    result.x = e.x;
    result.y = Game::m_render_height - e.y;

    if (m_events.size() > 1000000) {
      // Even though I'm clearing the events here, 
      // the flags remain intact.
      m_events.clear();
    }
  }
  
  return result;
}

bool Input::tapped() {
  if (m_end_tap_index != 0) {
    bool result = true;
    for (uint32_t i = m_begin_tap_index + 1; i < m_end_tap_index; ++i) {
      if (m_events[i].type == Event::Type::Move) {
        result = false;
        break;
      }
    }

    m_begin_tap_index = 0;
    m_end_tap_index = 0;
    return result;
  }

  return false;
}

bool Input::isScreenPressed() {
  return m_screen_pressed;
}