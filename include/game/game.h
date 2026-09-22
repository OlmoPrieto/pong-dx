#pragma once

#include <chrono>
#include <vector>

#include "base/types.h"
#include "game/ball.h"
#include "game/paddle.h"

struct SNetStream;

class IGame
{
public:
  static constexpr uint32 sm_uWindowWidth  = 1260u;
  static constexpr uint32 sm_uWindowHeight = 580u;

  static inline CVector2D sm_v2RightPlayerPos = { sm_uWindowWidth - 8.0f * 4.0f , sm_uWindowHeight - 32.0f * 4.0f };
  static inline CVector2D sm_v2LeftPlayerPos  = { 8.0f * 4.0f                   , 32.0f * 4.0f };
  
  static inline CVector2D sm_v2PaddleSize = { 16.0f, 64.0f };
  static inline CVector2D sm_v2BallSize = { 32.0f, 32.0f };
};

struct SGameState
{
  bool Write(SNetStream* _pStream);
  bool Read(SNetStream* _pStream);

  SGameState& operator=(const SGameState& _oOther);
  bool IsValid() const { return m_uFrame != UINT64_MAX; }

  std::vector<CVector2D> m_vctBallsPos;
  std::vector<CVector2D> m_vctBallsVel;
  CVector2D m_av2PlayersPos[2] = { CVector2D(), CVector2D() };
  uint64 m_uFrame = UINT64_MAX;
  std::chrono::time_point<std::chrono::high_resolution_clock> m_oTimeReceived;
  uint8 m_uNumBalls = 0u;
};