#include "game/server_paddle.h"

#include <limits>
#include <cmath>
#include <cassert>

#include "game/gameserver.h"
#include "game/server_ball.h"

// ------------------------

CServerPaddle::CServerPaddle()
{

}

// ------------------------

CServerPaddle::~CServerPaddle()
{

}

// ------------------------

void CServerPaddle::Init()
{
  
}

// ------------------------

void CServerPaddle::Update(float _fDt)
{
  
}

// ------------------------

void CServerPaddle::CheckCollisions(CServerBall* _pBall)
{
  uint32 uPrevContactFrames = m_uContactFrames;

  float fPosDiff = m_v2Pos.y - m_v2PrevPos.y;
  bool bAccelerating = fabsf(fPosDiff) > m_v2Size.y * 0.1f;

  bool bDetattched = false;

  const CVector2D v2BallPos = _pBall->m_v2Pos;
  const CVector2D v2BallHalfSize = _pBall->m_v2HalfSize;
  const float fBallRadius = _pBall->m_fRadius;
  if (v2BallPos.x + fBallRadius > m_v2Pos.x - m_v2HalfSize.x &&
    v2BallPos.x - fBallRadius < m_v2Pos.x + m_v2HalfSize.x &&
    v2BallPos.y + fBallRadius > m_v2Pos.y - m_v2HalfSize.y &&
    v2BallPos.y - fBallRadius < m_v2Pos.y + m_v2HalfSize.y
    )
  {
    // If contacted
    if (m_uContactFrames == 0u)
    {
      m_v2BallStoredVelocity = _pBall->m_v2Velocity;
      m_v2PrevPos = m_v2Pos;
    }

    ++m_uContactFrames;

    m_fAccVelocityOnContact = fPosDiff;

    _pBall->m_v2Velocity.x = 0.0f;
    _pBall->m_v2Velocity.y = 0.0f;

    // -----
    if (m_uContactFrames >= m_uMaxContactFrames)
    {
      bDetattched = true;
    }
  }
  else
  {
    // If not contacted
    if (uPrevContactFrames > 0u)
    {
      bDetattched = true;
    }
  }

  if (bDetattched)
  {
    // Horizontal Speed: reverse trajectory of the ball and
    //  add a bit of velocity depending on the friction with the paddle
    float fBallXVelocity = m_v2BallStoredVelocity.x;
    float fSpeedX = _pBall->m_v2Pos.x > CGameServer::sm_uWindowWidth * 0.5f ? -fBallXVelocity : fBallXVelocity;
    fSpeedX *= 1.025f; // Always increase speed a little on each hit
    float fMaxDisplacement = m_v2Size.x * 3.0f;
    float fClampedPaddlePosDiff = Clamp(fPosDiff, -fMaxDisplacement, fMaxDisplacement);
    _pBall->m_v2Velocity.x = fSpeedX * Remap(fClampedPaddlePosDiff, -fMaxDisplacement, fMaxDisplacement,
      1.25f, 2.0f) * std::copysign(1.0f, m_v2BallStoredVelocity.x);

    // Dettatch the ball from the paddle to not collide anymore
    if (_pBall->m_v2Pos.x > CGameServer::sm_uWindowWidth * 0.5f)
    {
      _pBall->m_v2Pos.x = m_v2Pos.x - m_v2HalfSize.x - _pBall->m_v2HalfSize.x - 1.0f;
    }
    else
    {
      _pBall->m_v2Pos.x = m_v2Pos.x + m_v2HalfSize.x + _pBall->m_v2HalfSize.x + 1.0f;
    }


    // Vertical Speed
    // Positive fPosDiff is downwards | Negative fPosDiff is upwards
    _pBall->m_v2Acceleration.y = fPosDiff * (float)m_uContactFrames * 500.0f;

    m_uContactFrames = 0u;
    m_v2BallStoredVelocity.x = 0.0f;
    m_v2BallStoredVelocity.y = 0.0f;

    m_v2BallStoredVelocity = CVector2D::v2ZERO;
  }
}

// ------------------------

// ------------------------
// ------------------------
// ------------------------