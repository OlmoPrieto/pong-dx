#include "game/gameserver.h"

#include <cassert>
#include <chrono>
#include <thread>

#include "base/network_serialization.h"
#include "enet/enet.h"
#include "game/gameclient.h"
#include "game/network_protocol.h"
#include "game/network_server.h"

// ------------------------

CGameServer::CGameServer()
{
  m_vctBalls.push_back(CServerBall());
  //m_oPaddle0.m_v2Pos = CVector2D(sm_uWindowWidth - 8.0f * 4.0f, sm_uWindowHeight - 32.0f * 4.0f);
  //m_oPaddle1.m_v2Pos = CVector2D(8.0f * 4.0f, 32.0f * 4.0f );
  m_oPaddle0.m_v2Pos = CVector2D(sm_uWindowWidth - 8.0f * 4.0f, sm_uWindowHeight - 32.0f * 4.0f);
  m_oPaddle1.m_v2Pos = CVector2D(8.0f * 4.0f, 32.0f * 4.0f);
  m_oGameState.m_uNumBalls = 1u;
  m_oGameState.m_vctBallsPos.push_back(CVector2D::v2ZERO);
  m_oGameState.m_vctBallsVel.push_back(CVector2D::v2ZERO);
  m_oGameState.m_v2Player0Pos = m_oPaddle0.m_v2Pos;
  m_oGameState.m_v2Player1Pos = m_oPaddle1.m_v2Pos;
}

// ------------------------

CGameServer::~CGameServer()
{

}

// ------------------------

CGameServer::CGameServer(CGameServer&& _oGame) noexcept
{
  m_oGameState = std::move(_oGame.m_oGameState);
  m_vctBalls = std::move(_oGame.m_vctBalls);
  m_pNetworkServer = std::move(_oGame.m_pNetworkServer);
  m_bGameStarted = std::move(_oGame.m_bGameStarted);
  m_bGameEnded = std::move(_oGame.m_bGameEnded);
}

// ------------------------

CGameServer& CGameServer::operator=(CGameServer&& _oGame) noexcept
{
  m_oGameState = std::move(_oGame.m_oGameState);
  m_vctBalls = std::move(_oGame.m_vctBalls);
  m_pNetworkServer = std::move(_oGame.m_pNetworkServer);
  m_bGameStarted = std::move(_oGame.m_bGameStarted);
  m_bGameEnded = std::move(_oGame.m_bGameEnded);

  return *this;
}

// ------------------------

void CGameServer::Init()
{
  
}

// ------------------------

void CGameServer::Begin(CNetworkServer* _pNetworkServer, CClientConnection* _pClient0, CClientConnection* _pClient1)
{
  m_pNetworkServer = _pNetworkServer;
  m_pClient0 = _pClient0;
  m_pClient1 = _pClient1;
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
    m_oPaddle0.CheckCollisions(&m_vctBalls[i]);
    m_oPaddle1.CheckCollisions(&m_vctBalls[i]);
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

  if (m_pNetworkServer->SendToClient(&oStream, m_pClient0, EMsgPriority::LOW))
  {
    //printf("%llu: Server sent data to client0\n", m_uLogicTick);
  }
  if (m_pNetworkServer->SendToClient(&oStream, m_pClient1, EMsgPriority::LOW))
  {
    //printf("%llu: Server sent data to client1\n", m_uLogicTick);
  }
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

  m_oGameState.m_v2Player0Pos = m_oPaddle0.m_v2Pos;
  m_oGameState.m_v2Player1Pos = m_oPaddle1.m_v2Pos;
}

// ------------------------
// ------------------------
// ------------------------