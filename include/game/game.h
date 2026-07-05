#pragma once

#include <chrono>
#include <vector>

#include "base/types.h"
#include "game/ball.h"
#include "game/paddle.h"

struct SNetStream;

class IGame
{
};

struct SGameState
{
  bool Write(SNetStream* _pStream);
  bool Read(SNetStream* _pStream);

  SGameState& operator=(const SGameState& _oOther);
  bool IsValid() const { return m_uFrame != UINT64_MAX; }

  std::vector<CVector2D> m_vctBallsPos;
  std::vector<CVector2D> m_vctBallsVel;
  CVector2D m_v2Player0Pos;
  CVector2D m_v2Player1Pos;
  uint64 m_uFrame = UINT64_MAX;
  std::chrono::time_point<std::chrono::high_resolution_clock> m_oTimeReceived;
  uint8 m_uNumBalls = 0u;
};

class CGame : public IGame
{
public:
  CGame(void);
  ~CGame();
  CGame(const CGame& _oGame) = delete;
  CGame(CGame&& _oGame) = delete;
  CGame& operator =(const CGame& _oGame) = delete;
  CGame& operator =(CGame&& _oGame) = delete;

  void ProcessInput();
  void Update(float _fDt);
  void Draw();

  const std::vector<CBall>& GetBalls() const { return m_vctBalls; }

  static constexpr uint32 sm_uWindowWidth  = 1260u;
  static constexpr uint32 sm_uWindowHeight = 580u;

  uint64 m_uLogicTick = 0u;
  constexpr static float m_fTargetFrameTime = 1.0f / 60.0f;

private:
  std::vector<CBall> m_vctBalls;
  CPaddle m_oPaddle0;
  CPaddle m_oPaddle1;
};