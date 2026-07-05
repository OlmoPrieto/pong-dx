#pragma once

#include <vector>

#include "game/game.h"
#include "game/clienthandle.h"
#include "base/types.h"
#include "game/server_ball.h"
#include "game/paddle.h"

class CGameClient;
struct _ENetPeer;
typedef struct _ENetPeer ENetPeer;

class CGameServer : public IGame
{
public:
  CGameServer();
  ~CGameServer();
  CGameServer(const CGameServer& _oGame) = delete;
  CGameServer(CGameServer&& _oGame) = delete;
  CGameServer& operator =(const CGameServer& _oGame) = delete;
  CGameServer& operator =(CGameServer&& _oGame) = delete;

  void Init();
  void Begin(ENetPeer* _pClient0, ENetPeer* _pClient1);
  void Loop();
  void Update(float _fDt);
  void SendGameState();
  bool GetGameEnded() const;
  bool GetGameStarted() const { return m_bGameStarted; }

  static constexpr uint32 sm_uWindowWidth = 1260u;
  static constexpr uint32 sm_uWindowHeight = 580u;

  uint64 m_uLogicTick = 0u;
  constexpr static float m_fTargetFrameTime = 1.0f / 60.0f;

private:
  void PrepareGameState();

  SGameState m_oGameState;
  std::vector<CServerBall> m_vctBalls;
  CClientHandle m_oClient0;
  CClientHandle m_oClient1;
  bool m_bGameStarted = false;
  bool m_bGameEnded = false;
};