#include <input.h>

#include <game.h>

std::deque<Input::Event> Input::m_events;

Input::Input() {
  // TODO: if implementing a custom queue, reserve space for elements here
}

Input::~Input() {

}

void Input::registerEvent(const Event& event) {
  m_events.push_back(event);
}

void Input::registerEvent(float x, float y, float time_stamp, int32_t type) {
  // TODO: for Type::Move, check if the mouse is down. If not, there is
  // no point in storing that event!!

  // if (type == Event::Type::Move) {
  //   if (m_events.size() > 0) {
  //     if (m_events[m_events.size() - 1].type == Event::Type::Up) {
  //       return;
  //     }

  //     Event* e = nullptr;
  //     bool greenlight = false;
  //     for (uint32_t i = m_events.size() - 2; i >= 0; --i) {
  //       e = &m_events[i];
  //       if (e->type == Event::Type::Down) {
  //         greenlight = true;
  //         m_events.emplace_back(x, y, time_stamp, (Event::Type)type);

  //         return;
  //       }
  //       else if (e->type == Event::Type::Up) {
  //         return;
  //       }
  //     }

  //     return;
  //   }
  // }

  m_events.emplace_back(x, y, time_stamp, (Event::Type)type);
}

Vec2 Input::getCursorPos() {
  Vec2 result;

  if (m_events.size() > 0) {//} && m_events.back().type == Event::Type::Move) {
    Event e(m_events.back());
    result.x = e.x;
    result.y = Game::m_render_height - e.y;

    // while (m_events.size() != 0) {
    //   m_events.pop();
    // }

    // if (m_events.size() > 0) {
    //   m_events.clear();
    // }
  }
  
  return result;
}

bool Input::tapped() {
  return false;
}

bool Input::isScreenPressed() {
  return false;
}