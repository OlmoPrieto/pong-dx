#pragma once

#include <memory>

#include "raylib/raylib.h"
#include "base/types.h"
#include "base/maths.h"

class IGame;
class CBall;

class CPaddle
{
public:
  CPaddle(IGame* _pOwnerGame, const CVector2D& _v2Pos, bool _bPlayerControlled);
  ~CPaddle();

  void Init();
  void Update(float _fDt);
  void Draw();

  void CheckCollisions(CBall* _pBall);

  CVector2D m_v2Pos;
  CVector2D m_v2PrevPos;
  CVector2D m_v2Velocity;
  CVector2D m_v2Size;
  CVector2D m_v2HalfSize;
  CVector2D m_v2BallStoredVelocity;
  float     m_fSpeed = 1.0f;
  float     m_fDirection = 0.0f;
  float     m_fAccVelocityOnContact = 0.0f;
  float     m_fAccTime = 0.0f;
  uint32    m_uMaxContactFrames = 4u;
  uint32    m_uContactFrames = 0u;

private:
  void UpdatePlayer(float _fDt);
  void UpdateAI(float _fDt);
  const CBall* GetClosestBall();

  std::shared_ptr<Texture2D> m_spTexture;
  void (CPaddle::* m_pfncUpdate)(float);

  IGame* m_pOwnerGame = nullptr;
};