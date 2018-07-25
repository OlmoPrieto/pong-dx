#include <input.h>

#include <game.h>

std::queue<Input::Event> Input::m_events;

Input::Input() {
  //m_events.reserve(1000);
}

Input::~Input() {

}

void Input::registerEvent(const Event& event) {
  m_events.push(event);
}

void Input::registerEvent(float x, float y, float time_stamp, int32_t type) {
  // TODO: for Type::Move, check if the mouse is down. If not, there is
  // no point in storing that event!!

  m_events.emplace(x, y, time_stamp, (Event::Type)type);
}

Vec2 Input::getCursorPos() {
  Vec2 result;

  if (m_events.size() > 0) {
    Event e(m_events.back());
    result.x = e.x;
    result.y = Game::m_render_height - e.y; // TODO: check if this works in PC as well

    while (m_events.size() != 0) {
      m_events.pop();
    }
  }
  
  return result;
}

bool Input::tapped() {
  return false;
}

bool Input::isScreenPressed() {
  return false;
}