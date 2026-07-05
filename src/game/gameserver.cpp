#include "game/gameserver.h"

#include <cassert>
#include <chrono>
#include <thread>

#include "base/network_serialization.h"
#include "enet/enet.h"
#include "game/gameclient.h"
#include "game/network_protocol.h"

// ------------------------

CGameServer::CGameServer()
{
  m_vctBalls.push_back(CServerBall());
  m_oGameState.m_uNumBalls = 1u;
  m_oGameState.m_vctBallsPos.push_back(CVector2D::v2ZERO);
  m_oGameState.m_vctBallsVel.push_back(CVector2D::v2ZERO);
}

// ------------------------

CGameServer::~CGameServer()
{

}

// ------------------------

void CGameServer::Init()
{
  
}

// ------------------------

void CGameServer::Begin(ENetPeer* _pClient0, ENetPeer* _pClient1)
{
  m_oClient0.Setup(_pClient0);
  m_oClient1.Setup(_pClient1);

  m_bGameStarted = true;
}

// ------------------------

void CGameServer::Loop()
{
  constexpr std::chrono::duration<float, std::milli> oTargetFrameTime(16.6666666f);

  if (m_bGameStarted && !m_bGameEnded)
  {
    std::chrono::time_point<std::chrono::high_resolution_clock> oFrameStart = std::chrono::high_resolution_clock::now();

    // Receive from clients
    // Update
    Update(oTargetFrameTime.count() * 0.001f);
  
    // Prepare game state
    PrepareGameState();

    // Send to clients
    SendGameState();

    // Sleep for remaining time
    const std::chrono::time_point<std::chrono::high_resolution_clock> oFrameEnd = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<float, std::milli> oProcessTime = oFrameEnd - oFrameStart;
    if (oProcessTime < oTargetFrameTime)
    {
      std::this_thread::sleep_for(oTargetFrameTime - oProcessTime);
    }

    ++m_uLogicTick;
  }
}

// ------------------------

void CGameServer::Update(float _fDt)
{
  for (uint32 i = 0; i < m_vctBalls.size(); ++i)
  {
    m_vctBalls[i].Update(_fDt);
  }
}

// ------------------------

void CGameServer::SendGameState()
{
  SNetStream oStream;
  byte pBuffer[NET_MAX_PACKET_SIZE];
  memset(pBuffer, 0, NET_MAX_PACKET_SIZE);
  InitNetStream(&oStream, pBuffer, NET_MAX_PACKET_SIZE);
  m_oGameState.Write(&oStream);

  if (m_oClient1.Send(&oStream, EMsgPriority::LOW))
  {
    //printf("%llu: Server sent data to client1\n", m_uLogicTick);
  }
  m_oClient0.Send(&oStream, EMsgPriority::LOW);
}

// ------------------------

bool CGameServer::GetGameEnded() const
{
  return m_bGameEnded;
}

// ------------------------

void CGameServer::PrepareGameState()
{
  m_oGameState.m_uNumBalls = m_vctBalls.size();
  for (uint32 i = 0; i < m_vctBalls.size(); ++i)
  {
    m_oGameState.m_vctBallsPos[i] = m_vctBalls[i].m_v2Pos;
  }
  for (uint32 i = 0; i < m_vctBalls.size(); ++i)
  {
    m_oGameState.m_vctBallsVel[i] = m_vctBalls[i].m_v2Velocity;
  }

  m_oGameState.m_uFrame = m_uLogicTick;

  
}

// ------------------------
// ------------------------
// ------------------------