#include "game/paddle.h"

#include <limits>
#include <cmath>
#include <cassert>

#include "game/game.h"
#include "game/ball.h"

// ------------------------

CPaddle::CPaddle(IGame* _pOwnerGame, const CVector2D& _v2Pos, bool _bPlayerControlled)
  : m_v2Pos(_v2Pos)
  , m_pOwnerGame(_pOwnerGame)
{
  if (_bPlayerControlled)
    m_pfncUpdate = &CPaddle::UpdatePlayer;
  else
    m_pfncUpdate = &CPaddle::UpdateAI;
}

// ------------------------

CPaddle::~CPaddle()
{

}

// ------------------------

void CPaddle::Init()
{
  assert(!m_spTexture);
  if (!m_spTexture)
  {
    m_spTexture = std::make_shared<Texture2D>(LoadTexture("assets/paddle_basic.png"));

    m_v2Size.x = m_spTexture->width;
    m_v2Size.y = m_spTexture->height;
    m_v2HalfSize = m_v2Size * 0.5f;
  }
}

// ------------------------

void CPaddle::Update(float _fDt)
{
  (this->*m_pfncUpdate)(_fDt);
}

// ------------------------

void CPaddle::Draw()
{
  DrawTextureEx(*m_spTexture.get(), 
   Vector2{m_v2Pos.x - m_v2HalfSize.x, 
     m_v2Pos.y - m_v2HalfSize.y},
     0.0f, 1.0f, WHITE);
}

// ------------------------

void CPaddle::CheckCollisions(CBall* _pBall)
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
    float fSpeedX = _pBall->m_v2Pos.x > CGame::sm_uWindowWidth * 0.5f ? -fBallXVelocity : fBallXVelocity;
    fSpeedX *= 1.025f; // Always increase speed a little on each hit
    float fMaxDisplacement = m_v2Size.x * 3.0f;
    float fClampedPaddlePosDiff = Clamp(fPosDiff, -fMaxDisplacement, fMaxDisplacement);
    _pBall->m_v2Velocity.x = fSpeedX * Remap(fClampedPaddlePosDiff, -fMaxDisplacement, fMaxDisplacement,
    1.25f, 2.0f) * std::copysign(1.0f, m_v2BallStoredVelocity.x);

    // Dettatch the ball from the paddle to not collide anymore
    if (_pBall->m_v2Pos.x > CGame::sm_uWindowWidth * 0.5f)
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

void CPaddle::UpdatePlayer(float _fDt)
{
  const float fPrevPosY = m_v2Pos.y;
  m_v2Pos.y = GetMouseY();
  if (fPrevPosY < m_v2Pos.y)
  {
    m_fDirection = 1.0f;
  }
  else if (fPrevPosY > m_v2Pos.y)
  {
    m_fDirection = -1.0f;
  }
  else 
  {
    m_fDirection = 0.0f;
  }
}

// ------------------------

void CPaddle::UpdateAI(float _fDt)
{
  const float fPrevPosY = m_v2Pos.y;
  const CBall* pBall = GetClosestBall();
  if (pBall)
  {
    if (m_uContactFrames == 0u)
    {
      m_fDirection = fPrevPosY < m_v2Pos.y ? 1.0f : fPrevPosY > m_v2Pos.y ? -1.0f : 0.0f;
      if (GetRandom(0.0f, 1.0f) >= 0.3f)
      {
        if (m_fDirection == 0.0f)
          m_fDirection = GetRandom(-1.0f, 1.0f);

        m_fDirection *= -1.0f;
      }

      m_v2Pos.y = pBall->m_v2Pos.y;
    }
    else if (m_uContactFrames > 0u)
    {
      m_v2Pos.y += GetRandom(0.5f, m_v2HalfSize.y) * m_fDirection;
    }
  }
}

// ------------------------

const CBall* CPaddle::GetClosestBall()
{
  const std::vector<CBall>& vctBalls = ((CGame*)m_pOwnerGame)->GetBalls();

  float fClosestX = std::numeric_limits<float>::max();
  const CBall* pClosestBall = nullptr;

  for (uint32 i = 0; i < vctBalls.size(); ++i)
  {
    const float fX = vctBalls[i].m_v2Pos.x;
    if (fX < fClosestX)
    {
      fClosestX = fX;
      pClosestBall = &vctBalls[i];
    }
  }

  return pClosestBall;
}

// ------------------------

// ------------------------
// ------------------------
// ------------------------