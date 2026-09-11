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

  WriteFloat32(_pStream, m_av2PlayersPos[0].x);
  WriteFloat32(_pStream, m_av2PlayersPos[0].y);

  WriteFloat32(_pStream, m_av2PlayersPos[1].x);
  WriteFloat32(_pStream, m_av2PlayersPos[1].y);

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

  ReadFloat32(_pStream, &m_av2PlayersPos[0].x);
  ReadFloat32(_pStream, &m_av2PlayersPos[0].y);

  ReadFloat32(_pStream, &m_av2PlayersPos[1].x);
  ReadFloat32(_pStream, &m_av2PlayersPos[1].y);

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