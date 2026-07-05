#include "game/ball.h"

#include <cassert>

#include "game/game.h"

// ------------------------

CBall::CBall()
{
  //m_v2Pos = m_v2Size;
  m_v2Pos = { 150.0f, 100.0f };

  m_v2Velocity.x = GetRandomValue(100.0f, 300.0f);
  m_v2Velocity.y = GetRandomValue(100.0f, 150.0f);

  m_uId = sm_uBallIdCounter++;
}

// ------------------------

CBall::~CBall()
{
  
}

// ------------------------

void CBall::UnloadResources()
{
  UnloadTexture(*m_spTexture.get());
}

// ------------------------

void CBall::Init()
{
  assert(!m_spTexture);

  // Image img = LoadImage("assets/ball_basic.png");
  // ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
  // Texture2D ballTex = LoadTextureFromImage(img);
  // m_spTexture = std::make_shared<Texture2D>(ballTex);
  m_spTexture = std::make_shared<Texture2D>(LoadTexture("assets/ball_basic.png"));
  printf("ball creation w:%u h:%d\n", m_spTexture->width, m_spTexture->height);
  m_v2Size.x = m_spTexture->width;
  m_v2Size.y = m_spTexture->height;
  m_v2HalfSize = m_v2Size * 0.5f;
  m_fRadius = m_v2HalfSize.x;
}

// ------------------------

void CBall::Update(float _fDt)
{
#define TEST_FIXED_PHYSICS 0
#if TEST_FIXED_PHYSICS
  m_fAccTime += _fDt;

  constexpr float fFixedTime = CGame::m_fTargetFrameTime;

  while (m_fAccTime >= fFixedTime)
  {
    m_v2PrevPos = m_v2Pos;
    m_v2Pos += (m_v2Velocity * m_fSpeed * fFixedTime) + 0.5f * m_v2Acceleration * (fFixedTime * fFixedTime);

    m_fAccTime -= fFixedTime;
  }
#endif // TEST_FIXED_PHYSICS

  // Apply physics
  m_v2Pos += (m_v2Velocity * m_fSpeed * _fDt) + 0.5f * m_v2Acceleration * (_fDt * _fDt);

  bool bReapplyPhysics = false;
  // Check with screen boundaries
  //  Sides
  if (m_v2Pos.x + m_v2HalfSize.x > CGame::sm_uWindowWidth || m_v2Pos.x - m_v2HalfSize.x < 0)
  {
    m_v2Velocity.x *= -1.0f;
    m_v2Acceleration.y *= -1.0f;
    bReapplyPhysics = true;
  }
  //  Vertical
  else if (m_v2Pos.y + m_v2HalfSize.y > CGame::sm_uWindowHeight || m_v2Pos.y - m_v2HalfSize.y < 0)
  {
    m_v2Velocity.y *= -1.0f;
    m_v2Acceleration.y *= -1.0f;
    bReapplyPhysics = true;
  }

  if (bReapplyPhysics)
  {
    m_v2Pos += (m_v2Velocity * m_fSpeed * _fDt) + 0.5f * m_v2Acceleration * (_fDt * _fDt);
  }

  // Reduce acceleration ("air" friction)
  m_v2Acceleration.y *= 0.95f;
  float fPrevAccY = m_v2Acceleration.y;
  if (m_v2Acceleration.y >= -0.001f && m_v2Acceleration.y <= 0.001f)
  {
    m_v2Acceleration.y = 0.0f;
  }
}

// ------------------------

void CBall::Draw()
{
  // DrawTextureEx(*m_spTexture.get(), 
  //   Vector2{m_v2Pos.x - m_v2HalfSize.x, 
  //   m_v2Pos.y - m_v2HalfSize.y},
  //   0.0f, 1.0f, WHITE);

  DrawTextureV(*m_spTexture, Vector2{m_v2Pos.x - m_v2HalfSize.x,
    m_v2Pos.y - m_v2HalfSize.y},
    WHITE);

  // const float fAlpha = m_fAccTime / CGame::m_fTargetFrameTime;
  // const CVector2D v2RenderPos = m_v2PrevPos + fAlpha * (m_v2Pos - m_v2PrevPos);
  // DrawTextureEx(*m_spTexture.get(), 
  //   Vector2{v2RenderPos.x - m_v2HalfSize.x, 
  //   v2RenderPos.y - m_v2HalfSize.y},
  //   0.0f, 1.0f, WHITE);
}

// ------------------------

// ------------------------

// ------------------------