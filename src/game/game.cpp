#include "game/game.h"

#include <random>
#include <chrono>

#include "raylib/raylib.h"
#include "base/network_serialization.h"
#include "game/network_protocol.h"

// ------------------------------------------------
// ------------------------------------------------

bool SGameState::Write(SNetStream* _pStream)
{
  if (_pStream->m_uOffset == 0u)
  {
    SPacketHeader oHeader;
    WriteHeader(_pStream, EMsgType::GAME_STATE);
  }

  assert(_pStream->m_uOffset == GetPacketHeaderSize());

  WriteUint64(_pStream, m_uFrame);

  WriteFloat32(_pStream, m_v2Player0Pos.x);
  WriteFloat32(_pStream, m_v2Player0Pos.y);

  WriteFloat32(_pStream, m_v2Player1Pos.x);
  WriteFloat32(_pStream, m_v2Player1Pos.y);

  WriteUint8(_pStream, m_uNumBalls);
  for (uint32 i = 0; i < m_uNumBalls; ++i)
  {
    WriteFloat32(_pStream, m_vctBallsPos[i].x);
    WriteFloat32(_pStream, m_vctBallsPos[i].y);
  }
  for (uint32 i = 0; i < m_uNumBalls; ++i)
  {
    WriteFloat32(_pStream, m_vctBallsVel[i].x);
    WriteFloat32(_pStream, m_vctBallsVel[i].y);
  }

  return true;
}

// ------------------------

bool SGameState::Read(SNetStream* _pStream)
{
  if (_pStream->m_uOffset == 0u)
  {
    SPacketHeader oHeader;
    ReadHeader(_pStream, &oHeader);
    assert(oHeader.m_uMsgType == EMsgType::GAME_STATE);
  }

  assert(_pStream->m_uOffset == GetPacketHeaderSize());

  uint64 uCurrentFrame = m_uFrame;
  ReadUint64(_pStream, &m_uFrame);

  ReadFloat32(_pStream, &m_v2Player0Pos.x);
  ReadFloat32(_pStream, &m_v2Player0Pos.y);

  ReadFloat32(_pStream, &m_v2Player1Pos.x);
  ReadFloat32(_pStream, &m_v2Player1Pos.y);

  ReadUint8(_pStream, &m_uNumBalls);
  assert(m_uNumBalls == m_vctBallsPos.size());
  for (uint32 i = 0; i < m_uNumBalls; ++i)
  {
    ReadFloat32(_pStream, &m_vctBallsPos[i].x);
    ReadFloat32(_pStream, &m_vctBallsPos[i].y);
  }
  for (uint32 i = 0; i < m_uNumBalls; ++i)
  {
    ReadFloat32(_pStream, &m_vctBallsVel[i].x);
    ReadFloat32(_pStream, &m_vctBallsVel[i].y);
  }

  return true;
}

// ------------------------

SGameState& SGameState::operator=(const SGameState& _oOther)
{
  if (m_uNumBalls != _oOther.m_uNumBalls)
  {
    m_vctBallsPos.reserve(_oOther.m_uNumBalls);
    m_vctBallsVel.reserve(_oOther.m_uNumBalls);

    for (uint32 i = m_vctBallsPos.size(); i < _oOther.m_uNumBalls; ++i)
    {
      m_vctBallsPos.push_back(CVector2D::v2ZERO);
      m_vctBallsVel.push_back(CVector2D::v2ZERO);
    }
  }

  m_uNumBalls = _oOther.m_uNumBalls;

  for (uint32 i = 0; i < _oOther.m_uNumBalls; ++i)
  {
    m_vctBallsPos[i] = _oOther.m_vctBallsPos[i];
  }

  for (uint32 i = 0; i < _oOther.m_uNumBalls; ++i)
  {
    m_vctBallsVel[i] = _oOther.m_vctBallsVel[i];
  }


  m_uFrame = _oOther.m_uFrame;
  m_oTimeReceived = _oOther.m_oTimeReceived;

  return *this;
}

// ------------------------

// ------------------------------------------------
// ------------------------------------------------

CGame::CGame(void)
  : m_oPaddle0(this, { sm_uWindowWidth - 8.0f * 4.0f, sm_uWindowHeight - 32.0f * 4.0f }, false)
  , m_oPaddle1(this, { 8.0f * 4.0f, 32.0f * 4.0f }, false)
{
  for (uint32 i = 0; i < 1; ++i)
  {
    m_vctBalls.push_back(CBall());
  }
}

// ------------------------

CGame::~CGame()
{
  for (uint32 i = 0; i < 1; ++i)
  {
    m_vctBalls[i].UnloadResources();
  }
}

// ------------------------

void CGame::ProcessInput()
{
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) // Accelerate balls
  {
    for (uint32 i = 0; i < m_vctBalls.size(); ++i)
    {
      m_vctBalls[i].m_fSpeed *= 5.0f;
    }
  }
  if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) // Decelerate balls
  {
    for (uint32 i = 0; i < m_vctBalls.size(); ++i)
    {
      m_vctBalls[i].m_fSpeed /= 5.0f;
    }
  }
  if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) // Reset balls
  {
    for (uint32 i = 0; i < m_vctBalls.size(); ++i)
    {
      m_vctBalls[i].m_fSpeed = 1.0f;
      m_vctBalls[i].m_v2Pos = { GetRandom(32.0f, 128.0f), GetRandom(32.0f, 128.0f)};
      m_vctBalls[i].m_v2Acceleration.x = 0.0f;
      m_vctBalls[i].m_v2Acceleration.y = 0.0f;
    }
  }
  if (IsKeyPressed(KEY_SPACE)) // Create new ball
  {
    m_vctBalls.push_back(CBall());
  }
  if (IsKeyPressed(KEY_R)) // Reset Balls
  {
    for (uint32 i = 0; i < m_vctBalls.size(); ++i)
    {
      m_vctBalls[i].m_fSpeed = 1.0f;
      m_vctBalls[i].m_v2Velocity.x = GetRandomValue(100.0f, 300.0f);
      m_vctBalls[i].m_v2Velocity.y = GetRandomValue(100.0f, 150.0f);
      m_vctBalls[i].m_v2Acceleration.x = 0.0f;
      m_vctBalls[i].m_v2Acceleration.y = 0.0f;
      m_vctBalls[i].m_v2Pos = { GetRandom(32.0f, 128.0f), GetRandom(32.0f, 128.0f)};
    }
  }
  if (IsKeyPressed(KEY_M)) // Reverse first ball
  {
    m_vctBalls[0u].m_v2Velocity.x = -200.0f;
  }
}

// ------------------------

void CGame::Update(float _fDt)
{
  m_oPaddle0.Update(_fDt);
  m_oPaddle1.Update(_fDt);

  for (uint32 i = 0; i < m_vctBalls.size(); ++i)
  {
    m_vctBalls[i].Update(_fDt);
    m_oPaddle0.CheckCollisions(&m_vctBalls[i]);
    m_oPaddle1.CheckCollisions(&m_vctBalls[i]);
  }

  ++m_uLogicTick;
}

// ------------------------

void CGame::Draw()
{
  m_oPaddle0.Draw();
  m_oPaddle1.Draw();
  for (uint32 i = 0; i < m_vctBalls.size(); ++i)
  {
    m_vctBalls[i].Draw();
  }
}

// ------------------------
// ------------------------
// ------------------------