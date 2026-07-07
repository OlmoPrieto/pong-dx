#pragma once

#include <vector>

#include "game/game.h"
#include "game/serverhandle.h"
#include "base/types.h"
#include "game/ball.h"
#include "game/paddle.h"

struct _ENetPeer;
typedef struct _ENetPeer ENetPeer;
class CNetworkClient;

class CGameClient : public IGame
{
public:
  class CClientBall
  {
  public:
    CClientBall();
    ~CClientBall();

    void Init();
    void UnloadResources();
    void Draw();

    CVector2D m_v2Pos;
    CVector2D m_v2Velocity;

  private:
    std::shared_ptr<Texture2D> m_spTexture;
  };

  CGameClient(void);
  ~CGameClient();
  CGameClient(const CGameClient& _oGame) = delete;
  CGameClient(CGameClient&& _oGame) = delete;
  CGameClient& operator =(const CGameClient& _oGame) = delete;
  CGameClient& operator =(CGameClient&& _oGame) = delete;

  void Init();  // Initialize state, create window
  void End();   // End game
  void Begin(CNetworkClient* _pNetworkClient); // Can start the match
  void Loop();
  void OnGameStateReceived(SNetStream* _pStream);
  void ProcessInput();
  void Update(float _fDt);
  void Draw();
  bool WantClose();
  bool GetGameStarted() const { return m_bGameStarted; }

  const std::vector<CClientBall>& GetBalls() const { return m_vctBalls; }

  static constexpr uint32 sm_uWindowWidth = 1260u;
  static constexpr uint32 sm_uWindowHeight = 580u;

  uint64 m_uLogicTick = 0u;
  constexpr static float m_fTargetFrameTime = 1.0f / 60.0f;

private:
  void UpdateBalls(float _fDt);
  void UpdateGameState();

  SGameState m_oGameState;
  SGameState m_oPrevGameState;
  std::vector<CClientBall> m_vctBalls;
  CPaddle m_oPlayerPaddle;
  CPaddle m_oEnemyPaddle;
  CNetworkClient* m_pNetworkClient = nullptr;
  uint32 m_uClientId = UINT_MAX;
  bool m_bWantClose = false;
  bool m_bGameStarted = false;
};