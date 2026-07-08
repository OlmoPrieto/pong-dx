#include "game/client_paddle.h"

#include <limits>
#include <cmath>
#include <cassert>

#include "game/gameclient.h"

// ------------------------

CClientPaddle::CClientPaddle(IGame* _pOwnerGame, const CVector2D& _v2Pos, bool _bPlayerControlled)
  : m_v2Pos(_v2Pos)
  , m_pOwnerGame(_pOwnerGame)
{
  if (_bPlayerControlled)
    m_pfncUpdate = &CClientPaddle::UpdatePlayer;
  else
    m_pfncUpdate = &CClientPaddle::UpdateAI;
}

// ------------------------

CClientPaddle::~CClientPaddle()
{

}

// ------------------------

void CClientPaddle::Init()
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

void CClientPaddle::Update(float _fDt)
{
  (this->*m_pfncUpdate)(_fDt);
}

// ------------------------

void CClientPaddle::Draw()
{
  DrawTextureEx(*m_spTexture.get(),
    Vector2{ m_v2Pos.x - m_v2HalfSize.x,
      m_v2Pos.y - m_v2HalfSize.y },
    0.0f, 1.0f, WHITE);
}

// ------------------------

void CClientPaddle::UpdatePlayer(float _fDt)
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

void CClientPaddle::UpdateAI(float _fDt)
{
  const float fPrevPosY = m_v2Pos.y;
  const CClientBall* pBall = GetClosestBall();
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

const CClientBall* CClientPaddle::GetClosestBall() const
{
  const std::vector<CClientBall>& vctBalls = ((CGameClient*)m_pOwnerGame)->GetBalls();

  float fClosestX = std::numeric_limits<float>::max();
  const CClientBall* pClosestBall = nullptr;

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